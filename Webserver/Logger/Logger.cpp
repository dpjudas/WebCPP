
#include "Precomp.h"
#include "Logger.h"
#include "Text/UTF16.h"
#include <mutex>
#ifdef WIN32
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <Windows.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

class ConsoleLoggerImpl;

namespace
{
	std::list<LogMessage> messages;
	std::mutex mutex;
	ConsoleLoggerImpl* console;
	thread_local std::string source;
	thread_local std::string moduleName;
}

class ConsoleLoggerImpl : public ConsoleLogger
{
public:
#ifdef WIN32
	ConsoleLoggerImpl()
	{
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (GetConsoleMode(stdOut, &mode))
		{
			SetConsoleMode(stdOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
		}
		console = this;
	}

	~ConsoleLoggerImpl()
	{
		console = nullptr;
		SetConsoleMode(stdOut, mode);
	}

	HANDLE stdOut = {};
	DWORD mode;
#else
	ConsoleLoggerImpl()
	{
		console = this;
	}

	~ConsoleLoggerImpl()
	{
		console = nullptr;
	}
#endif

	void printMessage(const LogMessage& message)
	{
		std::string level;
		int color = 0;
		switch (message.level)
		{
		default: level = std::to_string((int)message.level); break;
		case LogLevel::error: level = "error"; color = 91; break;
		case LogLevel::warning: level = "warning"; color = 93; break;
		case LogLevel::info: level = "info"; break;
		case LogLevel::verbose: level = "verbose"; color = 90; break;
		}

		std::string source = message.source;
		source.resize(40, ' ');

		std::string text;
		text += colorEscape(90) + "[" + resetEscape();
		if (color != 0)
			text += colorEscape(color);
		text += level;
		if (color != 0)
			text += resetEscape();
		text += colorEscape(90) + "]" + resetEscape();
		text += std::string(std::max((int)(10 - level.length()), 0), ' ');
		text += " ";
		text += source;
		text += " ";
		text += message.text;
		text += newLine();
		writeOutput(text);
	}

	std::string resetEscape() { return "\x1b[0m"; }
	std::string colorEscape(int color) { return "\x1b[" + std::to_string(color) + "m"; }
	std::string deleteCharacterEscape(int count) { return "\x1b[" + std::to_string(count) + "P"; }
	std::string eraseCharacterEscape(int count) { return "\x1b[" + std::to_string(count) + "X"; }
	std::string cursorBackward(int count) { return "\x1b[" + std::to_string(count) + "D"; }

	std::string newLine()
	{
#ifdef WIN32
		return "\r\n";
#else
		return "\n";
#endif
	}

	std::string inputNewLine()
	{
#ifdef WIN32
		return "\r";
#else
		return "\n";
#endif
	}

	void waitForInput()
	{
#ifdef WIN32
		WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), INFINITE);
#else
		fd_set rfds;
		timeval tv;
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		tv.tv_sec = 365 * 24 * 60 * 60;
		tv.tv_usec = 0;
		select(1, &rfds, NULL, NULL, &tv);
#endif
	}

	void writeOutput(const std::string& text)
	{
		if (text.empty())
			return;

		// Convert any type of string to valid Windows \r\n output
		writeBuffer.clear();
		for (char c : text)
		{
			if (c != '\r' && c != '\n')
			{
				writeBuffer.push_back(c);
			}
			else if (c == '\n')
			{
				writeBuffer.push_back('\r');
				writeBuffer.push_back('\n');
			}
		}

#ifdef WIN32
		std::wstring text16 = to_utf16(writeBuffer);
		size_t pos = 0;
		while (pos < text16.size())
		{
			DWORD written = 0;
			if (!WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), text16.data() + pos, (DWORD)(text16.size() - pos), &written, nullptr))
				break;
			pos += written;
		}
#else
		size_t pos = 0;
		while (pos < text.size())
		{
			auto byteswritten = write(1, text.data() + pos, text.size() - pos);
			if (byteswritten <= 0)
				break;
			pos += byteswritten;
		}
#endif
	}

	std::string writeBuffer;
};

void logMessage(LogLevel level, std::string text)
{
	LogMessage message;
	message.level = level;
	message.source = source;
	message.text = std::move(text);

	std::unique_lock lock(mutex);
	if (console)
		console->printMessage(message);
	messages.push_back(std::move(message));
}

LoggerRange Logger::getMessages()
{
	std::unique_lock lock(mutex);
	return LoggerRange(messages.begin(), messages.end());
}

std::unique_ptr<ConsoleLogger> ConsoleLogger::create()
{
	return std::make_unique<ConsoleLoggerImpl>();
}

LoggerSource::LoggerSource(std::string name)
{
	oldName = std::move(source);
	source = std::move(name);
}

LoggerSource::~LoggerSource()
{
	source = std::move(oldName);
}
