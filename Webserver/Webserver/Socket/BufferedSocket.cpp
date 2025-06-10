
#include "Precomp.h"
#include "BufferedSocket.h"

BufferedSocket::BufferedSocket(SocketHandle* sock, std::atomic<bool>* stopFlag) : Sock(sock), StopFlag(stopFlag)
{
}

char BufferedSocket::ReadChar()
{
	if (Pos != Length)
		return Data[Pos++];

	Pos = 0;
	Length = Sock->RecvData(Data.data(), Data.size(), *StopFlag);
	if (Length == 0)
		throw HttpErrorException();
	return Data[Pos++];
}

std::string BufferedSocket::ReadLine()
{
	std::string line;
	while (true)
	{
		char c = ReadChar();
		if (c == '\r')
		{
			c = ReadChar();
			if (c == '\n')
			{
				break;
			}
			else
			{
				line.push_back(c);
			}
		}
		else if (c == '\n')
		{
			break;
		}
		else
		{
			line.push_back(c);
		}
	}
	return line;
}

size_t BufferedSocket::ReadContent(void* dest, size_t size)
{
	if (Pos != Length)
	{
		size = std::min(size, Length - Pos);
		memcpy(dest, Data.data() + Pos, size);
		Pos += size;
		return size;
	}
	else
	{
		return Sock->RecvData(dest, size, *StopFlag);
	}
}

size_t BufferedSocket::SendData(const void* data, size_t size)
{
	return Sock->SendData(data, size, *StopFlag);
}
