
#include "Precomp.h"
#include "StringFormat.h"
#include "Text.h"
#include <stdexcept>

StringFormat::StringFormat(const std::string &format_string)
	: string(format_string)
{
	bool arg_mode = false;
	unsigned int arg_start = 0;
	int arg_value = 0;
	std::string::size_type index, size;
	size = string.size();
	for (index = 0; index < size; index++)
	{
		if (!arg_mode)
		{
			if (string[index] == '%')
			{
				arg_mode = true;
				arg_value = 0;
				arg_start = (int)index;
			}
		}
		else
		{
			switch (string[index])
			{
			case '%':
				if (arg_start == index - 1)
				{
					arg_mode = false;
					string = string.substr(0, index) + string.substr(index + 1);
					index--;
					size--;
				}
				else
				{
					create_arg(arg_value, arg_start, (int)(index - arg_start));
					arg_start = (int)index;
					arg_value = 0;
				}
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				arg_value *= 10;
				arg_value += string[index] - '0';
				break;
			default:
				if (arg_start != index - 1)
					create_arg(arg_value, arg_start, (int)(index - arg_start));
				arg_mode = false;
				break;
			}
		}
	}

	if (arg_mode && arg_start != size - 1)
		create_arg(arg_value, arg_start, (int)(size - arg_start));
}

const std::string &StringFormat::result() const
{
	return string;
}

void StringFormat::set_arg(int index, const std::string_view& text)
{
	set_arg(index, std::string(text));
}

void StringFormat::set_arg(int index, const std::string &text)
{
	if (index >= (int)args.size())
		return;

	ArgPosition pos = args[index];
	if (pos.length == -1)
		return;

	int delta_size = ((int)text.length()) - pos.length;
	string = string.substr(0, pos.start) + text + string.substr(pos.start + pos.length);
	args[index].length = (int)text.length();

	std::vector<ArgPosition>::size_type i, size;
	size = args.size();
	for (i = 0; i < size; i++)
	{
		if (args[i].start > pos.start)
			args[i].start += delta_size;
	}
}

void StringFormat::set_arg(int index, int value, int min_length)
{
	std::string t = Text::to_string(value);
	if ((int)t.length() < min_length)
		t = std::string(min_length - t.length(), '0') + t;
	set_arg(index, t);
}

void StringFormat::set_arg(int index, unsigned int value, int min_length)
{
	std::string t = Text::to_string(value);
	if ((int)t.length() < min_length)
		t = std::string(min_length - t.length(), '0') + t;
	set_arg(index, t);
}

void StringFormat::set_arg(int index, long long value, int min_length)
{
	std::string t = Text::to_string(value);
	if ((int)t.length() < min_length)
		t = std::string(min_length - t.length(), '0') + t;
	set_arg(index, t);
}

void StringFormat::set_arg(int index, unsigned long long value, int min_length)
{
	std::string t = Text::to_string(value);
	if ((int)t.length() < min_length)
		t = std::string(min_length - t.length(), '0') + t;
	set_arg(index, t);
}

void StringFormat::set_arg(int index, float value)
{
	set_arg(index, Text::to_string(value));
}

void StringFormat::set_arg(int index, double value)
{
	set_arg(index, Text::to_string(value));
}

void StringFormat::create_arg(int index, int start, int length)
{
	if (index > 256)
		throw std::runtime_error("Encountered more than 256 indexes in a formatted string!");

	ArgPosition pos(start, length);
	while (index >= (int)args.size())
		args.push_back(ArgPosition());

	args[index].start = start;
	args[index].length = length;
}
