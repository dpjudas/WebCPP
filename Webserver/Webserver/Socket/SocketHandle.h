#pragma once

#include <atomic>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "wsock32.lib")
#define socklen_t int
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define closesocket close
#endif

struct HttpErrorException {};

class SocketHandle
{
public:
	SocketHandle();
	SocketHandle(SOCKET handle);
	~SocketHandle();

	SOCKET handle = INVALID_SOCKET;

	size_t RecvData(void* data, size_t size, std::atomic<bool>& stopFlag);
	size_t SendData(const void* data, size_t size, std::atomic<bool>& stopFlag);

private:
	SocketHandle(const SocketHandle&) = delete;
};
