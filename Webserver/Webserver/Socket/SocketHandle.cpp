
#include "Precomp.h"
#include "SocketHandle.h"

SocketHandle::SocketHandle()
{
	handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (handle == INVALID_SOCKET)
		throw std::runtime_error("Could not create socket");
}

SocketHandle::SocketHandle(SOCKET handle) : handle(handle)
{
}

SocketHandle::~SocketHandle()
{
	if (handle != INVALID_SOCKET)
		closesocket(handle);
}

size_t SocketHandle::RecvData(void* data, size_t size, std::atomic<bool>& stopFlag)
{
	size = std::min(size, (size_t)0x7fffffff);
	for (int i = 0; !stopFlag && i < 30; i++)
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(handle, &rfds);

		timeval timeout = {};
		timeout.tv_sec = 1;
		int result = select((int)(handle + 1), &rfds, nullptr, nullptr, &timeout);
		if (result >= 0 && FD_ISSET(handle, &rfds))
		{
			int result = recv(handle, (char*)data, (int)size, 0);
			if (result < 0) throw HttpErrorException();
			return result;
		}
	}
	throw HttpErrorException();
}

size_t SocketHandle::SendData(const void* data, size_t size, std::atomic<bool>& stopFlag)
{
	size = std::min(size, (size_t)0x7fffffff);
	for (int i = 0; !stopFlag && i < 30; i++)
	{
		fd_set wfds;
		FD_ZERO(&wfds);
		FD_SET(handle, &wfds);

		timeval timeout = {};
		timeout.tv_sec = 1;
		int result = select((int)(handle + 1), nullptr, &wfds, nullptr, &timeout);
		if (result >= 0 && FD_ISSET(handle, &wfds))
		{
			int result = send(handle, (const char*)data, (int)size, 0);
			if (result < 0) throw HttpErrorException();
			return result;
		}
	}
	throw HttpErrorException();
}
