
#include "Precomp.h"
#include "SocketWebserver.h"
#include "Webserver/WebContext.h"
#include <mutex>

SocketWebserver::SocketWebserver()
{
#ifdef WIN32
	WSADATA wsaData = {};
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		throw std::runtime_error("WSAStartup failed");
#endif
}

SocketWebserver::~SocketWebserver()
{
	// Stop listen thread
	std::unique_lock lock(Mutex);
	StopFlag = true;
	lock.unlock();
	if (Thread.joinable())
		Thread.join();

	// Wait for all connection threads to end
	lock.lock();
	for (auto& client : Clients)
		client->StopFlag = true;
	while (!Clients.empty())
	{
		ClientsListEmpty.wait(lock);
	}
}

void SocketWebserver::start(const std::string& listenUrl)
{
	ListenSocket = std::make_unique<SocketHandle>();

	int flag = 1;
	setsockopt(ListenSocket->handle, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(int));

	sockaddr_in name = {};
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = htons(8080);
	int result = bind(ListenSocket->handle, (const sockaddr*)&name, sizeof(sockaddr_in));
	if (result != 0)
		throw std::runtime_error("Socket bind failed");

	result = listen(ListenSocket->handle, 50);
	if (result < 0)
		throw std::runtime_error("Socket listen failed");

	Thread = std::thread([this]() { ListenThreadMain(); });
}

void SocketWebserver::ListenThreadMain()
{
	std::unique_lock lock(Mutex);
	while (!StopFlag)
	{
		lock.unlock();

		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(ListenSocket->handle, &rfds);

		timeval timeout = {};
		timeout.tv_sec = 1;
		int result = select((int)(ListenSocket->handle + 1), &rfds, nullptr, nullptr, &timeout);

		lock.lock();

		if (result < 0 || StopFlag)
		{
			break;
		}
		else if (result >= 0)
		{
			if (FD_ISSET(ListenSocket->handle, &rfds))
			{
				socklen_t namelength = sizeof(sockaddr_in);
				sockaddr_in name = {};
				name.sin_family = AF_INET;
				SOCKET clientsocket = accept(ListenSocket->handle, (sockaddr*)&name, &namelength);
				if (clientsocket != INVALID_SOCKET)
				{
					Clients.push_back(std::make_unique<ConnectedClient>(this, clientsocket));
				}
			}
		}
	}
}

SocketWebserver::ConnectedClient::ConnectedClient(SocketWebserver* webserver, SOCKET handle) : Webserver(webserver), Socket(std::make_unique<SocketHandle>(handle))
{
	Thread = std::thread([this]() { ConnectionThreadMain(); });
}

SocketWebserver::ConnectedClient::~ConnectedClient()
{
	StopFlag = true;
	if (Thread.joinable())
		Thread.join();
}

void SocketWebserver::ConnectedClient::ConnectionThreadMain()
{
	try
	{
		BufferedSocket bufferedsocket(Socket.get(), &StopFlag);
		while (HandleRequest(bufferedsocket)) {}
	}
	catch (const HttpErrorException&)
	{
	}

	// Remove from list of connected clients
	std::unique_ptr<ConnectedClient> self;
	std::unique_lock lock(Webserver->Mutex);
	for (auto it = Webserver->Clients.begin(); it != Webserver->Clients.end(); ++it)
	{
		if (it->get() == this)
		{
			self = std::move(*it);
			Webserver->Clients.erase(it);
			break;
		}
	}
	if (Webserver->Clients.empty())
		Webserver->ClientsListEmpty.notify_all();
	Thread.detach();
}

bool SocketWebserver::ConnectedClient::HandleRequest(BufferedSocket& sock)
{
	WebContext webctx;

	std::string startLine = sock.ReadLine();

	size_t verbEnd = startLine.find(' ');
	if (verbEnd == std::string::npos)
		throw HttpErrorException();
	std::string verb = startLine.substr(0, verbEnd);

	size_t requestTargetStart = verbEnd + 1;
	size_t requestTargetEnd = startLine.find(' ', requestTargetStart);
	if (requestTargetEnd == std::string::npos)
		throw HttpErrorException();
	std::string requestTarget = startLine.substr(requestTargetStart, requestTargetEnd - requestTargetStart);
	std::string protocolVersion = startLine.substr(requestTargetEnd + 1);

	if (protocolVersion != "HTTP/1.0" && protocolVersion != "HTTP/1.1")
		throw HttpErrorException();

	if (verb == "GET")
		webctx.request.verb = WebRequestVerb::get;
	else if (verb == "POST")
		webctx.request.verb = WebRequestVerb::post;
	else if (verb == "HEAD")
		webctx.request.verb = WebRequestVerb::head;
	else if (verb == "OPTIONS")
		webctx.request.verb = WebRequestVerb::options;
	else if (verb == "DELETE")
		webctx.request.verb = WebRequestVerb::delete_;
	else if (verb == "PATCH")
		webctx.request.verb = WebRequestVerb::patch;
	else if (verb == "PUT")
		webctx.request.verb = WebRequestVerb::put;
	else
		throw HttpErrorException();

	while (true)
	{
		std::string line = sock.ReadLine();
		if (line.empty())
			break;

		size_t nameEnd = line.find(':');
		if (nameEnd == std::string::npos)
			throw HttpErrorException();

		size_t valueStart = line.find_first_not_of(' ', nameEnd + 1);
		if (valueStart == std::string::npos)
			throw HttpErrorException();

		webctx.request.headers[line.substr(0, nameEnd)] = line.substr(valueStart);
	}

	auto itHost = webctx.request.headers.find("Host");
	if (itHost == webctx.request.headers.end())
		throw HttpErrorException();

	auto itClose = webctx.request.headers.find("Connection");
	bool closeConnection = (itClose == webctx.request.headers.end()) || itClose->second != "keep-alive";

	webctx.request.url = WebRequestUrl("http://" + itHost->second + requestTarget);

	if (verb == "POST")
	{
		auto itContentLength = webctx.request.headers.find("Content-Length");
		if (itContentLength == webctx.request.headers.end())
			throw HttpErrorException(); // To do: implement chunked transfer

		int64_t contentLength = std::atoll(itContentLength->second.c_str());
		if (contentLength < 0 || contentLength > 4 * 1024 * 1024)
			throw HttpErrorException(); // To do: implement streaming large uploads

		webctx.request.content = DataBuffer::create(contentLength);
		size_t pos = 0;
		while (pos < webctx.request.content->size())
		{
			pos += sock.ReadContent(webctx.request.content->data() + pos, webctx.request.content->size() - pos);
		}
	}

	Webserver->processRequest(&webctx);

	size_t contentSize = webctx.response.content ? webctx.response.content->size() : 0;
	webctx.response.headers["Content-Length"] = std::to_string(contentSize);
	webctx.response.headers["Connection"] = closeConnection ? "close" : "keep-alive";

	std::string responseHeader = "HTTP/1.1 " + std::to_string(webctx.response.status.code) + " " + webctx.response.status.text + "\r\n";
	for (auto& header : webctx.response.headers)
	{
		responseHeader += header.first.toString() + ": " + header.second + "\r\n";
	}
	responseHeader += "\r\n";

	size_t pos = 0;
	while (pos < responseHeader.size())
	{
		pos += sock.SendData(responseHeader.data() + pos, responseHeader.size() - pos);
	}

	pos = 0;
	if (webctx.response.content)
	{
		while (pos < webctx.response.content->size())
		{
			pos += sock.SendData(webctx.response.content->data() + pos, webctx.response.content->size() - pos);
		}
	}

	return !closeConnection;
}
