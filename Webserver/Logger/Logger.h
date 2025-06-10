#pragma once

#include "Common/Text/StringFormat.h"
#include <list>

enum class LogLevel
{
	error,
	warning,
	info,
	verbose
};

void logMessage(LogLevel level, std::string text);

void logMessage(LogLevel level, const std::string& format, auto&&... args)
{
	logMessage(level, formatString(format, std::forward<decltype(args)>(args)...));
}

class LogMessage
{
public:
	LogLevel level = {};
	std::string source;
	std::string text;
};

class LoggerRange
{
public:
	LoggerRange(std::list<LogMessage>::iterator itBegin, std::list<LogMessage>::iterator itEnd) : itBegin(itBegin), itEnd(itEnd) {}

	std::list<LogMessage>::iterator begin() const { return itBegin; }
	std::list<LogMessage>::iterator end() const { return itEnd; }

private:
	std::list<LogMessage>::iterator itBegin, itEnd;
};

class Logger
{
public:
	static LoggerRange getMessages();
};

class ConsoleLogger
{
public:
	virtual ~ConsoleLogger() = default;

	static std::unique_ptr<ConsoleLogger> create();
};

class LoggerSource
{
public:
	LoggerSource(std::string name);
	~LoggerSource();

private:
	std::string oldName;
};
