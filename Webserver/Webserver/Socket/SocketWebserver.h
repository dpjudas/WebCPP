#pragma once

#include "Webserver/Webserver.h"
#include "SocketHandle.h"
#include "BufferedSocket.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class SocketWebserver : public Webserver
{
public:
	SocketWebserver();
	~SocketWebserver();

	void start(const std::string& listenUrl) override;

	struct ConnectedClient
	{
		ConnectedClient(SocketWebserver* webserver, SOCKET handle);
		~ConnectedClient();

		void ConnectionThreadMain();
		bool HandleRequest(BufferedSocket& sock);

		SocketWebserver* Webserver = nullptr;
		std::unique_ptr<SocketHandle> Socket;
		std::thread Thread;
		std::atomic<bool> StopFlag;
	};

	void ListenThreadMain();

	std::unique_ptr<SocketHandle> ListenSocket;
	std::vector<std::unique_ptr<ConnectedClient>> Clients;
	std::thread Thread;
	std::mutex Mutex;
	std::condition_variable ClientsListEmpty;
	bool StopFlag = false;
};
