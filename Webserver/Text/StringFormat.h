
#pragma once

class Guid;
class ShortGuid;

class StringFormat
{
public:
	StringFormat(const std::string &format_string);

	const std::string &result() const;

	void set_arg(int index, const std::string &text);
	void set_arg(int index, const std::string_view& text);
	void set_arg(int index, int value, int min_length = 0);
	void set_arg(int index, unsigned int value, int min_length = 0);
	void set_arg(int index, long long value, int min_length = 0);
	void set_arg(int index, unsigned long long value, int min_length = 0);
	void set_arg(int index, float value);
	void set_arg(int index, double value);

private:
	void create_arg(int index, int start, int length);

	std::string string;

	struct ArgPosition
	{
		ArgPosition() : start(0), length(-1) { }
		ArgPosition(int s, int l) : start(s), length(l) {}
		int start;
		int length;
	};

	std::vector<ArgPosition> args;
};

inline std::string formatString(const std::string &format)
{
	return format;
}

template <class Arg1>
std::string formatString(const std::string &format, Arg1 arg1)
{
	StringFormat f(format); f.set_arg(1, arg1); return f.result();
}

template <class Arg1, class Arg2>
std::string formatString(const std::string &format, Arg1 arg1, Arg2 arg2)
{
	StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); return f.result();
}

template <class Arg1, class Arg2, class Arg3>
std::string formatString(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3)
{
	StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); return f.result();
}

template <class Arg1, class Arg2, class Arg3, class Arg4>
std::string formatString(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
{
	StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); f.set_arg(4, arg4); return f.result();
}

template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
std::string formatString(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
{
	StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); f.set_arg(4, arg4); f.set_arg(5, arg5); return f.result();
}

template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
std::string formatString(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
{
	StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); f.set_arg(4, arg4); f.set_arg(5, arg5); f.set_arg(6, arg6); return f.result();
}

template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
std::string formatString(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
{
	StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); f.set_arg(4, arg4); f.set_arg(5, arg5); f.set_arg(6, arg6); f.set_arg(7, arg7); return f.result();
}
