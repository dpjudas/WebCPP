
#include "xml_tokenizer.h"
#include "xml_token.h"
#include "xml_tokenizer_impl.h"
#include <algorithm>
#include <utility>
#include <stdexcept>

#ifdef _MSC_VER
#pragma warning(disable: 4456)
#endif

std::shared_ptr<XmlTokenizer> XmlTokenizer::create(const std::string& data)
{
	return std::make_shared<XmlTokenizerImpl>(data);
}

/////////////////////////////////////////////////////////////////////////////

XmlTokenizerImpl::XmlTokenizerImpl(const std::string& data) : data(data)
{
	size = data.size();
	pos = 0;
}

bool XmlTokenizerImpl::eat_whitespace() const
{
	return _eat_whitespace;
}

void XmlTokenizerImpl::set_eat_whitespace(bool enable)
{
	_eat_whitespace = enable;
}

void XmlTokenizerImpl::next(XmlToken *out_token)
{
	out_token->type = XmlTokenType::null;
	out_token->variant = XmlTokenVariant::single;
	//	out_token->attributes.clear();
	while (!out_token->attributes.empty())
		out_token->attributes.pop_back();

	if (!next_text_node(out_token))
		next_tag_node(out_token);
}

bool XmlTokenizerImpl::next_text_node(XmlToken *out_token)
{
	while (pos < size && data[pos] != '<')
	{
		std::string::size_type start_pos = pos;
		std::string::size_type end_pos = data.find('<', start_pos);
		if (end_pos == data.npos) end_pos = size;
		pos = end_pos;

		std::string text;
		std::string text_orig = data.substr(start_pos, end_pos - start_pos);
		unescape(text, text_orig);
		if (_eat_whitespace)
		{
			text = trim_whitespace(text);
			if (text.empty())
				continue;
		}

		out_token->type = XmlTokenType::text;
		out_token->value = text;
		return true;
	}
	return false;
}

bool XmlTokenizerImpl::next_tag_node(XmlToken *out_token)
{
	if (pos == size || data[pos] != '<')
		return false;

	pos++;
	if (pos == size)
		XmlTokenizerImpl::throw_exception("Premature end of XML data!");

	// Try to early predict what sort of node it might be:
	bool closing = (data[pos] == '/');
	bool questionMark = (data[pos] == '?');
	bool exclamationMark = (data[pos] == '!');

	if (closing || questionMark || exclamationMark)
	{
		pos++;
		if (pos == size)
			XmlTokenizerImpl::throw_exception("Premature end of XML data!");
	}

	if (exclamationMark) // check for cdata section, comments or doctype
	{
		if (next_exclamation_mark_node(out_token))
			return true;
	}

	// Extract the tag name:
	std::string::size_type start_pos = pos;
	std::string::size_type end_pos = data.find_first_of(" \r\n\t?/>", start_pos);
	if (end_pos == data.npos)
		XmlTokenizerImpl::throw_exception("Premature end of XML data!");
	pos = end_pos;

	out_token->type = questionMark ? XmlTokenType::processing_instruction : XmlTokenType::element;
	out_token->variant = closing ? XmlTokenVariant::end : XmlTokenVariant::begin;
	out_token->name = data.substr(start_pos, end_pos - start_pos);

	if (out_token->type == XmlTokenType::processing_instruction)
	{
		// Strip whitespace:
		pos = data.find_first_not_of(" \r\n\t", pos);
		if (pos == data.npos)
			XmlTokenizerImpl::throw_exception("Premature end of XML data!");

		end_pos = data.find_first_of("?", pos);
		if (end_pos == data.npos)
			XmlTokenizerImpl::throw_exception("Premature end of XML data!");
		out_token->value = data.substr(pos, end_pos - pos);
		pos = end_pos;
	}
	else // out_token->type == XmlTokenType::element
	{
		// Check for possible attributes:
		while (true)
		{
			// Strip whitespace:
			pos = data.find_first_not_of(" \r\n\t", pos);
			if (pos == data.npos)
				XmlTokenizerImpl::throw_exception("Premature end of XML data!");

			// End of tag, stop searching for more attributes:
			if (data[pos] == '/' || data[pos] == '?' || data[pos] == '>')
				break;

			// Extract attribute name:
			std::string::size_type start_pos = pos;
			std::string::size_type end_pos = data.find_first_of(" \r\n\t=", start_pos);
			if (end_pos == data.npos)
				XmlTokenizerImpl::throw_exception("Premature end of XML data!");
			pos = end_pos;

			std::string attributeName = data.substr(start_pos, end_pos - start_pos);

			// Find seperator:
			pos = data.find_first_not_of(" \r\n\t", pos);
			if (pos == data.npos || pos == size - 1)
				XmlTokenizerImpl::throw_exception("Premature end of XML data!");
			if (data[pos++] != '=')
				XmlTokenizerImpl::throw_exception("XML error(s), parser confused at line " + std::to_string(get_line_number()) + " (tag=" + out_token->name + ", attributeName=" + attributeName + ")");

			// Strip whitespace:
			pos = data.find_first_not_of(" \r\n\t", pos);
			if (pos == data.npos)
				XmlTokenizerImpl::throw_exception("Premature end of XML data!");

			// Extract attribute value:
			std::string::value_type const * first_of = " \r\n\t";
			if (data[pos] == '"')
			{
				first_of = "\"";
				pos++;
				if (pos == size)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");
			}
			else
				if (data[pos] == '\'')
				{
					first_of = "'";
					pos++;
					if (pos == size)
						XmlTokenizerImpl::throw_exception("Premature end of XML data!");
				}

			start_pos = pos;
			end_pos = data.find_first_of(first_of, start_pos);
			if (end_pos == data.npos)
				XmlTokenizerImpl::throw_exception("Premature end of XML data!");

			std::string attributeValue;
			std::string attributeValueOrig = data.substr(start_pos, end_pos - start_pos);
			unescape(attributeValue, attributeValueOrig);

			pos = end_pos + 1;
			if (pos == size)
				XmlTokenizerImpl::throw_exception("Premature end of XML data!");

			// Finally apply attribute to token:
			out_token->attributes.push_back(XmlToken::Attribute(attributeName, attributeValue));
		}
	}

	// Check if its singular:
	if (data[pos] == '/' || data[pos] == '?')
	{
		out_token->variant = XmlTokenVariant::single;
		pos++;
		if (pos == size)
			XmlTokenizerImpl::throw_exception("Premature end of XML data!");
	}

	// Data stream should be ending now.
	if (data[pos] != '>')
		XmlTokenizerImpl::throw_exception("Error in XML stream, line " + std::to_string(get_line_number()) + " (expected end of tag)");
	pos++;

	return true;
}

bool XmlTokenizerImpl::next_exclamation_mark_node(XmlToken *out_token)
{
	if (pos + 2 >= size)
		XmlTokenizerImpl::throw_exception("Premature end of XML data!");

	if (data.compare(pos, 2, "--") == 0) // comment block
	{
		std::string::size_type start_pos = pos + 2;
		std::string::size_type end_pos = data.find("-->", start_pos);
		if (end_pos == data.npos)
			XmlTokenizerImpl::throw_exception("Premature end of XML data!");
		pos = end_pos + 3;

		std::string text;
		std::string text_orig = data.substr(start_pos, end_pos - start_pos);
		unescape(text, text_orig);
		if (_eat_whitespace)
			text = trim_whitespace(text);

		out_token->type = XmlTokenType::comment;
		out_token->variant = XmlTokenVariant::single;
		out_token->value = text;
		return true;
	}

	if (pos + 7 >= size)
		XmlTokenizerImpl::throw_exception("Premature end of XML data!");

	if (data.compare(pos, 7, "DOCTYPE") == 0)
	{
		// Strip whitespace:
		pos = data.find_first_not_of(" \r\n\t", pos + 7);
		if (pos == data.npos)
			XmlTokenizerImpl::throw_exception("Premature end of XML data!");

		// Find doctype name:				
		std::string::size_type name_start = pos;
		std::string::size_type name_end = data.find_first_of(" \r\n\t?/>", name_start);
		if (name_end == data.npos)
			XmlTokenizerImpl::throw_exception("Premature end of XML data!");
		pos = name_end;

		// Strip whitespace:
		pos = data.find_first_not_of(" \r\n\t", pos);
		if (pos == data.npos)
			XmlTokenizerImpl::throw_exception("Premature end of XML data!");

		std::string::size_type public_start = data.npos;
		std::string::size_type public_end = data.npos;
		std::string::size_type system_start = data.npos;
		std::string::size_type system_end = data.npos;
		std::string::size_type subset_start = data.npos;
		std::string::size_type subset_end = data.npos;

		// Look for possible external id:
		if (data[pos] != '[' && data[pos] != '>')
		{
			if (pos + 6 >= size)
				XmlTokenizerImpl::throw_exception("Premature end of XML data!");

			if (data.compare(pos, 6, "SYSTEM") == 0)
			{
				pos += 6;
				if (pos == size)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");

				// Strip whitespace:
				pos = data.find_first_not_of(" \r\n\t", pos);
				if (pos == data.npos)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");

				// Read system literal:
				std::string::value_type literal_char = data[pos];
				if (literal_char != '\'' && literal_char != '"')
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");

				system_start = pos + 1;
				system_end = data.find(literal_char, system_start);
				if (system_end == data.npos)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");
				pos = system_end + 1;
				if (pos >= size)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");
			}
			else if (data.compare(pos, 6, "PUBLIC") == 0)
			{
				pos += 6;
				if (pos == size)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");

				// Strip whitespace:
				pos = data.find_first_not_of(" \r\n\t", pos);
				if (pos == data.npos)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");

				// Read public literal:
				std::string::value_type literal_char = data[pos];
				if (literal_char != '\'' && literal_char != '"')
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");

				public_start = pos + 1;
				public_end = data.find(literal_char, public_start);
				if (public_end == data.npos)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");
				pos = public_end + 1;
				if (pos >= size)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");

				// Strip whitespace:
				pos = data.find_first_not_of(" \r\n\t", pos);
				if (pos == data.npos)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");

				// Read system literal:
				literal_char = data[pos];
				if (literal_char != '\'' && literal_char != '"')
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");

				system_start = pos + 1;
				system_end = data.find(literal_char, system_start);
				if (system_end == data.npos)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");
				pos = system_end + 1;
				if (pos >= size)
					XmlTokenizerImpl::throw_exception("Premature end of XML data!");
			}
			else
				XmlTokenizerImpl::throw_exception("Error in XML stream, line " + std::to_string(get_line_number()) + " (unknown external identifier type in DOCTYPE)");

			// Strip whitespace:
			pos = data.find_first_not_of(" \r\n\t", pos);
			if (pos == data.npos)
				XmlTokenizerImpl::throw_exception("Premature end of XML data!");
		}

		// Look for possible internal subset:
		if (data[pos] == '[')
		{
			subset_start = pos + 1;

			// Search for the end of the internal subset:
			// (to avoid parsing it, we search backwards)
			std::string::size_type end_pos = data.find('>', pos + 1);
			if (end_pos == data.npos)
				XmlTokenizerImpl::throw_exception("Premature end of XML data!");

			subset_end = data.rfind(']', end_pos);
			if (subset_end == data.npos)
				XmlTokenizerImpl::throw_exception("Error in XML stream, line " + std::to_string(get_line_number()) + " (expected end of internal subset in DOCTYPE)");

			pos = end_pos;
		}

		// Expect DOCTYPE tag to end now:
		if (data[pos] != '>')
			XmlTokenizerImpl::throw_exception("Error in XML stream, line " + std::to_string(get_line_number()) + " (expected end of DOCTYPE)");
		pos++;

		out_token->type = XmlTokenType::doctype;
		return true;
	}
	else if (data.compare(pos, 7, "[CDATA[") == 0)
	{
		std::string::size_type start_pos = pos + 7;
		std::string::size_type end_pos = data.find("]]>", start_pos);
		if (end_pos == data.npos)
			XmlTokenizerImpl::throw_exception("Premature end of XML data!");
		pos = end_pos + 3;

		std::string value = data.substr(start_pos, end_pos - start_pos);

		out_token->type = XmlTokenType::cdata;
		out_token->variant = XmlTokenVariant::single;
		out_token->value = value;
		return true;
	}
	else
	{
		XmlTokenizerImpl::throw_exception("Error in XML stream at position " + std::to_string(pos));
		return false;
	}
}

void XmlTokenizerImpl::throw_exception(const std::string &str)
{
	throw std::runtime_error(str);
}

int XmlTokenizerImpl::get_line_number()
{
	int line = 1;
	std::string::size_type tmp_pos = 0;

	std::string::const_iterator it;
	for (it = data.begin(); it != data.end() && tmp_pos <= pos; ++it, tmp_pos++)
	{
		if ((*it) == '\n')
			line++;
	}

	return line;
}

inline void XmlTokenizerImpl::unescape(std::string &unescaped, const std::string &text)
{
	static const std::string quot("&quot;");
	static const std::string apos("&apos;");
	static const std::string lt("&lt;");
	static const std::string gt("&gt;");
	static const std::string amp("&amp;");

	unescaped = text;
	unescape(unescaped, quot, '"');
	unescape(unescaped, apos, '\'');
	unescape(unescaped, lt, '<');
	unescape(unescaped, gt, '>');
	unescape(unescaped, amp, '&');
}

inline void XmlTokenizerImpl::unescape(std::string &text, const std::string &search, std::string::value_type replace)
{
	std::string::size_type read_pos = 0;
	std::string::size_type length = text.length();
	std::string::size_type search_length = search.length();
	while (true)
	{
		std::string::size_type next_match = text.find(search, read_pos);
		if (next_match == std::string::npos)
			break;

		std::string::size_type copy_size = length - (next_match + search_length);
		if (copy_size > 0)
			memmove(&text[next_match + 1], &text[next_match + search_length], copy_size * sizeof(std::string::value_type));
		text[next_match] = replace;
		length -= search_length - 1;
		read_pos = next_match + 1;
		if (read_pos > length)
			break;
	}
	text.resize(length);
}

inline std::string XmlTokenizerImpl::trim_whitespace(const std::string &text)
{
	std::string::size_type pos_start = text.find_first_not_of(" \t\r\n");
	if (pos_start == std::string::npos)
		return std::string();
	std::string::size_type pos_end = text.find_last_not_of(" \t\r\n");
	if (pos_end == std::string::npos)
		return std::string();
	return text.substr(pos_start, pos_end - pos_start + 1);
}
