
#include "xml_writer.h"
#include "xml_token.h"
#include "xml_writer_impl.h"

#ifdef _MSC_VER
#pragma warning(disable: 4458)
#endif

std::shared_ptr<XmlWriter> XmlWriter::create(std::function<void(const void* data, size_t size)> writeCallback)
{
	return std::make_shared<XmlWriterImpl>(writeCallback);
}

/////////////////////////////////////////////////////////////////////////////

XmlWriterImpl::XmlWriterImpl(std::function<void(const void* data, size_t size)> writeCallback) : writeCallback(writeCallback)
{
	str.reserve(4096);
	escaped_string.reserve(4096);
}

bool XmlWriterImpl::insert_whitespace() const
{
	return _insert_whitespace;
}

void XmlWriterImpl::set_insert_whitespace(bool enable)
{
	_insert_whitespace = enable;
}

void XmlWriterImpl::write(const XmlToken &token)
{
	// We are reusing a std::string here to build up a capacity that fits
	// all strings we write.
	str.clear();

	if (token.variant == XmlTokenVariant::end)
	{
		indent--;
	}

#ifdef WIN32
#define NEWLINE_STR "\r\n"
#else
#define NEWLINE_STR "\n"
#endif

	switch (token.type)
	{
	case XmlTokenType::null:
		return; // should this throw exception instead?

	case XmlTokenType::element:
		if (token.variant == XmlTokenVariant::begin || token.variant == XmlTokenVariant::single)
		{
			if (_insert_whitespace)
			{
				if (!first_token)
					str.append(NEWLINE_STR);
				str.append(indent, L'\t');
			}

			str.append("<");
			str.append(insert_escapes_fast(token.name));

			int size = (int)token.attributes.size();
			for (int i = 0; i < size; i++)
			{
				str.append(" ");
				str.append(token.attributes[i].first);
				str.append("=\"");
				str.append(insert_escapes_fast(token.attributes[i].second));
				str.append("\"");
			}

			if (token.variant == XmlTokenVariant::single)
			{
				str.append("/>");
				single_line_tag = false;
			}
			else
			{
				str.append(">");
				single_line_tag = true;
			}
		}
		else if (token.variant == XmlTokenVariant::end)
		{
			if (_insert_whitespace && !single_line_tag)
			{
				str.append(NEWLINE_STR);
				str.append(indent, L'\t');
			}
			str.append("</");
			str.append(insert_escapes_fast(token.name));
			str.append(">");
			single_line_tag = false;
		}
		break;

	case XmlTokenType::text:
		str.append(insert_escapes_fast(token.value));
		break;

	case XmlTokenType::cdata:
		str.append("<![CDATA[");
		str.append(token.value);
		str.append("]]>");
		break;

	case XmlTokenType::comment:
		if (_insert_whitespace)
		{
			if (!first_token)
				str.append(NEWLINE_STR);
			str.append(indent, L'\t');
		}
		str.append("<!--");
		str.append(token.value);
		str.append("-->");
		break;

	case XmlTokenType::processing_instruction:
		if (_insert_whitespace && !first_token)
			str.append(NEWLINE_STR);

		str.append("<?");
		str.append(token.name);
		if (!token.value.empty())
		{
			str.append(" ");
			str.append(token.value);
		}
		str.append("?>");
		break;

	case XmlTokenType::doctype:
		return; // To do: implement storing doctype in XmlTokenizer and write it back out here
	}

	first_token = false;

	if (token.variant == XmlTokenVariant::begin)
	{
		indent++;
	}

	writeCallback(str.data(), str.size());
}

std::string XmlWriterImpl::insert_escapes_fast(const std::string &str)
{
	static std::string const amp("&amp;");
	static std::string const quot("&quot;");
	static std::string const apos("&apos;");
	static std::string const lt("&lt;");
	static std::string const gt("&gt;");

	escaped_string = str;
	std::string::size_type pos = 0;
	while (pos < escaped_string.size())
	{
		switch (escaped_string[pos])
		{
		case '&':
			escaped_string.replace(pos, 1, amp);
			pos += amp.size();
			break;
		case '\'':
			escaped_string.replace(pos, 1, apos);
			pos += apos.size();
			break;
		case '\"':
			escaped_string.replace(pos, 1, quot);
			pos += quot.size();
			break;
		case '<':
			escaped_string.replace(pos, 1, lt);
			pos += lt.size();
			break;
		case '>':
			escaped_string.replace(pos, 1, gt);
			pos += gt.size();
			break;
		default:
			++pos;
			break;
		}
	}
	return escaped_string;
}
