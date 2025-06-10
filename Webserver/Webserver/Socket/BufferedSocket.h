#pragma once

#include "SocketHandle.h"
#include <array>

class BufferedSocket
{
public:
	BufferedSocket(SocketHandle* sock, std::atomic<bool>* stopFlag);

	char ReadChar();
	std::string ReadLine();

	size_t ReadContent(void* dest, size_t size);
	size_t SendData(const void* data, size_t size);

private:
	SocketHandle* Sock = nullptr;
	std::atomic<bool>* StopFlag = nullptr;
	size_t Pos = 0, Length = 0;
	std::array<char, 1024> Data;
};
