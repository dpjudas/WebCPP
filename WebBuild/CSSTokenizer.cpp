
#include "Precomp.h"
#include "CSSTokenizer.h"

class CSSTokenizerImpl : public CSSTokenizer
{
public:
	CSSTokenizerImpl(std::string text);

	void read(CSSToken& out_token, bool eat_whitespace, bool eat_comments = true) override;
	std::vector<CSSToken> read_property_value(CSSToken& token, bool& out_important_flag) override;

private:
	void read_token(CSSToken& out_token);
	void peek_token(CSSToken& out_token);

	void read_atkeyword(CSSToken& out_token);
	void read_hash(CSSToken& out_token);
	void read_cdo(CSSToken& out_token);
	void read_cdc(CSSToken& out_token);
	void read_comment(CSSToken& out_token);
	void read_uri(CSSToken& out_token);
	void read_function(CSSToken& out_token);
	void read_whitespace(CSSToken& out_token);
	void read_includes(CSSToken& out_token);
	void read_dashmatch(CSSToken& out_token);
	void read_number_type(CSSToken& out_token);
	size_t read_ident(size_t p, std::string& out_ident);
	size_t read_name(size_t p, std::string& out_ident);
	inline size_t read_nmstart(size_t p, std::string::value_type& out_c);
	inline size_t read_nmchar(size_t p, std::string::value_type& out_c);
	size_t read_string(size_t p, std::string& out_str, std::string::value_type str_char) const;
	size_t read_invalid(size_t p) const;
	size_t read_uri_nonquoted_string(size_t p, std::string& out_str) const;
	inline static bool is_whitespace(std::string::value_type c);
	inline static bool equal_caseless(const std::string& a, const std::string& b);

	std::string doc;
	size_t pos = 0;
};

std::unique_ptr<CSSTokenizer> CSSTokenizer::create(std::string text)
{
	return std::make_unique<CSSTokenizerImpl>(std::move(text));
}

std::vector<CSSToken> CSSTokenizer::tokenize(std::string text)
{
	std::vector<CSSToken> tokens;
	CSSTokenizerImpl tokenizer(std::move(text));
	CSSToken token;
	bool first = true;
	while (true)
	{
		tokenizer.read(token, false);
		if (token.type == CSSTokenType::null)
			break;
		if (!first || token.type != CSSTokenType::whitespace)
		{
			first = false;
			tokens.push_back(token);
		}
	}
	while (!tokens.empty() && tokens.back().type == CSSTokenType::whitespace)
		tokens.pop_back();
	return tokens;
}

CSSTokenizerImpl::CSSTokenizerImpl(std::string text) : doc(std::move(text))
{
}

void CSSTokenizerImpl::read(CSSToken &token, bool eat_whitespace, bool eat_comments)
{
	do
	{
		read_token(token);
		if (eat_comments && (token.type == CSSTokenType::whitespace || token.type == CSSTokenType::comment))
		{
			if (token.type == CSSTokenType::comment)
			{
				token.type = CSSTokenType::whitespace;
				token.value = " ";
			}
			CSSToken next_token;
			while (true)
			{
				peek_token(next_token);
				if (next_token.type != CSSTokenType::whitespace && next_token.type != CSSTokenType::comment)
					break;
				read_token(next_token);
				if (next_token.type == CSSTokenType::comment)
				{
					next_token.type = CSSTokenType::whitespace;
					next_token.value = " ";
				}
				token.value += next_token.value;
			}
		}
	} while((eat_whitespace && token.type == CSSTokenType::whitespace));
}

std::vector<CSSToken> CSSTokenizerImpl::read_property_value(CSSToken &token, bool &important_flag)
{
	std::vector<CSSToken> value_tokens;
	important_flag = false;

	// Remove any possible whitespace at the beginning of the property value:
	if (token.type == CSSTokenType::whitespace)
		read(token, true);

	int curly_count = 0;
	while (true)
	{
		if (token.type == CSSTokenType::null)
		{
			break;
		}
		else if (token.type == CSSTokenType::curly_brace_begin)
		{
			curly_count++;
		}
		else if (token.type == CSSTokenType::curly_brace_end)
		{
			curly_count--;
			if (curly_count <= 0)
				break;
		}
		else if (token.type == CSSTokenType::semi_colon)
		{
			if (curly_count == 0)
				break;
		}

		//if (token.type == CSSTokenType::uri)
		//	token.value = make_absolute_uri(token.value, base_uri);

		value_tokens.push_back(token);
		read(token, false);
	}

	// Remove any possible whitespace at the end of the property:
	while (!value_tokens.empty() && value_tokens.back().type == CSSTokenType::whitespace)
		value_tokens.pop_back();

	// Remove the !important flag if found:
	size_t tokens_size = value_tokens.size();
	if (tokens_size >= 2 &&
		value_tokens[tokens_size - 2].type == CSSTokenType::delim && value_tokens[tokens_size - 2].value == "!" &&
		value_tokens[tokens_size - 1].type == CSSTokenType::ident && equal_caseless(value_tokens[tokens_size - 1].value, "important"))
	{
		important_flag = true;
		value_tokens.pop_back();
		value_tokens.pop_back();
	}
	else if (tokens_size >= 3 &&
		value_tokens[tokens_size - 3].type == CSSTokenType::delim && value_tokens[tokens_size - 3].value == "!" &&
		value_tokens[tokens_size - 2].type == CSSTokenType::whitespace &&
		value_tokens[tokens_size - 1].type == CSSTokenType::ident && equal_caseless(value_tokens[tokens_size - 1].value, "important"))
	{
		important_flag = true;
		value_tokens.pop_back();
		value_tokens.pop_back();
		value_tokens.pop_back();
	}

	if (important_flag)
	{
		// Remove any possible whitespace at the end of the property:
		while (!value_tokens.empty() && value_tokens.back().type == CSSTokenType::whitespace)
			value_tokens.pop_back();
	}

	return value_tokens;
}

void CSSTokenizerImpl::peek_token(CSSToken& out_token)
{
	size_t cur_pos = pos;
	read_token(out_token);
	pos = cur_pos;
}

void CSSTokenizerImpl::read_token(CSSToken& token)
{
	token.type = CSSTokenType::null;
	token.dimension.clear();
	token.value.clear();
	token.offset = pos;
	if (pos >= doc.length())
		return;
	switch (doc[pos])
	{
	case '@':
		read_atkeyword(token);
		break;
	case '#':
		read_hash(token);
		break;
	case '<':
		read_cdo(token);
		break;
	case '-':
		read_cdc(token);
		break;
	case ':':
		token.type = CSSTokenType::colon;
		pos++;
		break;
	case ';':
		token.type = CSSTokenType::semi_colon;
		pos++;
		break;
	case '{':
		token.type = CSSTokenType::curly_brace_begin;
		pos++;
		break;
	case '}':
		token.type = CSSTokenType::curly_brace_end;
		pos++;
		break;
	case '(':
		token.type = CSSTokenType::bracket_begin;
		pos++;
		break;
	case ')':
		token.type = CSSTokenType::bracket_end;
		pos++;
		break;
	case '[':
		token.type = CSSTokenType::square_bracket_begin;
		pos++;
		break;
	case ']':
		token.type = CSSTokenType::square_bracket_end;
		pos++;
		break;
	case '/':
		read_comment(token);
		break;
	case 'u':
	case 'U':
		read_uri(token);
		// read_unicode_range(token);
		break;
	case ' ':
	case '\t':
	case '\r':
	case '\n':
	case '\f':
		read_whitespace(token);
		break;
	case '~':
		read_includes(token);
		break;
	case '|':
		read_dashmatch(token);
		break;
	case '.':
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
		read_number_type(token);
		break;
	}
	if (token.type != CSSTokenType::null)
		return;

	read_function(token);
	if (token.type != CSSTokenType::null)
		return;

	pos = read_ident(pos, token.value);
	if (!token.value.empty())
	{
		token.type = CSSTokenType::ident;
		return;
	}

	if (doc[pos] == '"')
	{
		size_t end_pos = read_string(pos + 1, token.value, '"');
		if (end_pos != pos + 1)
		{
			token.type = CSSTokenType::string;
			pos = end_pos;
			return;
		}
		else
		{
			token.type = CSSTokenType::invalid;
			pos = read_invalid(pos + 1);
			return;
		}
	}
	else if (doc[pos] == '\'')
	{
		size_t end_pos = read_string(pos + 1, token.value, '\'');
		if (end_pos != pos + 1)
		{
			token.type = CSSTokenType::string;
			pos = end_pos;
			return;
		}
		else
		{
			token.type = CSSTokenType::invalid;
			pos = read_invalid(pos + 1);
			return;
		}
	}
	else
	{
		token.type = CSSTokenType::delim;
		token.value.append(1, doc[pos]);
		pos++;
	}
}

void CSSTokenizerImpl::read_number_type(CSSToken& token)
{
	bool dot_encountered = false;
	size_t end_pos = pos;
	while (end_pos < doc.length())
	{
		if (doc[end_pos] == '.' && !dot_encountered)
		{
			dot_encountered = true;
		}
		else if (doc[end_pos] < '0' || doc[end_pos] > '9')
		{
			break;
		}
		end_pos++;
	}

	if (!(pos + 1 == end_pos && dot_encountered))
	{
		token.value = doc.substr(pos, end_pos - pos);
		if (end_pos < doc.length() && doc[end_pos] == '%')
		{
			token.type = CSSTokenType::percentage;
			pos = end_pos + 1;
		}
		else
		{
			end_pos = read_ident(end_pos, token.dimension);
			if (!token.dimension.empty())
				token.type = CSSTokenType::dimension;
			else
				token.type = CSSTokenType::number;
			pos = end_pos;
		}
	}
}

void CSSTokenizerImpl::read_atkeyword(CSSToken& token)
{
	if (pos + 2 <= doc.length() && doc[pos] == '@')
	{
		pos = read_ident(pos + 1, token.value);
		if (!token.value.empty())
			token.type = CSSTokenType::atkeyword;
	}
}

void CSSTokenizerImpl::read_hash(CSSToken& token)
{
	if (pos + 2 <= doc.length() && doc[pos] == '#')
	{
		pos = read_name(pos + 1, token.value);
		if (!token.value.empty())
			token.type = CSSTokenType::hash;
	}
}

size_t CSSTokenizerImpl::read_ident(size_t p, std::string& out_ident)
{
	out_ident.clear();
	std::string::value_type c = 0;
	size_t len = 0;
	if (p + 2 <= doc.length() && doc[p] == '-')
	{
		len = read_nmstart(p + 1, c);
		if (len > 0)
		{
			out_ident.append(1, '-');
			len++;
		}
	}
	else
	{
		len = read_nmstart(p, c);
	}
	if (len > 0)
	{
		out_ident.append(1, c);
		size_t end_pos = p + len;
		while (end_pos < doc.length())
		{
			len = read_nmchar(end_pos, c);
			if (len == 0)
				break;
			out_ident.append(1, c);
			end_pos += len;
		}
		p = end_pos;
	}
	return p;
}

size_t CSSTokenizerImpl::read_name(size_t p, std::string& out_ident)
{
	out_ident.clear();
	std::string::value_type c;
	size_t len = read_nmchar(p, c);
	if (len > 0)
	{
		out_ident.append(1, c);
		size_t end_pos = p + len;
		while (end_pos < doc.length())
		{
			len = read_nmchar(end_pos, c);
			if (len == 0)
				break;
			out_ident.append(1, c);
			end_pos += len;
		}
		p = end_pos;
	}
	return p;
}

void CSSTokenizerImpl::read_cdo(CSSToken& token)
{
	if (pos + 4 <= doc.length() && doc[pos] == '<' && doc[pos + 1] == '!' && doc[pos + 2] == '-' && doc[pos + 3] == '-')
	{
		token.type = CSSTokenType::cdo;
		token.value.clear();
		pos += 4;
	}
}

void CSSTokenizerImpl::read_cdc(CSSToken& token)
{
	if (pos + 3 <= doc.length() && doc[pos] == '-' && doc[pos + 1] == '-' && doc[pos + 2] == '>')
	{
		token.type = CSSTokenType::cdc;
		token.value.clear();
		pos += 3;
	}
}

void CSSTokenizerImpl::read_comment(CSSToken& token)
{
	if (pos + 4 <= doc.length() && doc[pos] == '/' && doc[pos + 1] == '*')
	{
		size_t end_pos = pos + 3;
		while (end_pos < doc.length() && !(doc[end_pos] == '/' && doc[end_pos - 1] == '*'))
			end_pos++;
		if (end_pos < doc.length())
		{
			token.type = CSSTokenType::comment;
			pos = end_pos + 1;
		}
	}
}

void CSSTokenizerImpl::read_uri(CSSToken& token)
{
	if (pos + 5 <= doc.length() &&
		(doc[pos] == 'u' || doc[pos] == 'U') &&
		(doc[pos + 1] == 'r' || doc[pos + 1] == 'R') &&
		(doc[pos + 2] == 'l' || doc[pos + 2] == 'L') &&
		doc[pos + 3] == '(')
	{
		size_t end_pos = pos + 4;
		while (end_pos < doc.length() && is_whitespace(doc[end_pos]))
			end_pos++;
		if (end_pos < doc.length())
		{
			switch (doc[end_pos])
			{
			case '"': // string1
				end_pos = read_string(end_pos + 1, token.value, '"');
				break;
			case '\'': // string2
				end_pos = read_string(end_pos + 1, token.value, '\'');
				break;
			default: // ([!#$%&*-~]|{nonascii}|{escape})*
				end_pos = read_uri_nonquoted_string(end_pos, token.value);
				break;
			case ')':
				token.value.clear();
				break;
			}
		}
		while (end_pos < doc.length() && is_whitespace(doc[end_pos]))
			end_pos++;

		if (end_pos < doc.length() && doc[end_pos] == ')')
		{
			token.type = CSSTokenType::uri;
			pos = end_pos + 1;
		}
	}
}

void CSSTokenizerImpl::read_function(CSSToken& token)
{
	size_t end_pos = read_ident(pos, token.value);
	if (end_pos != pos)
	{
		if (end_pos + 1 <= doc.length() && doc[end_pos] == '(')
		{
			pos = end_pos + 1;
			token.type = CSSTokenType::function;
		}
	}
}

void CSSTokenizerImpl::read_whitespace(CSSToken& token)
{
	if (pos + 1 <= doc.length() && is_whitespace(doc[pos]))
	{
		token.type = CSSTokenType::whitespace;
		token.value.clear();
		size_t end_pos = pos + 1;
		while (end_pos < doc.length() && is_whitespace(doc[end_pos]))
			end_pos++;
		pos = end_pos;
	}
}

void CSSTokenizerImpl::read_includes(CSSToken& token)
{
	if (pos + 2 <= doc.length() && doc[pos] == '~' && doc[pos + 1] == '=')
	{
		token.type = CSSTokenType::includes;
		token.value.clear();
		pos += 2;
	}
}

void CSSTokenizerImpl::read_dashmatch(CSSToken& token)
{
	if (pos + 2 <= doc.length() && doc[pos] == '|' && doc[pos + 1] == '=')
	{
		token.type = CSSTokenType::dashmatch;
		token.value.clear();
		pos += 2;
	}
}

size_t CSSTokenizerImpl::read_nmstart(size_t p, std::string::value_type& out_c)
{
	out_c = '?';
	if ((p < doc.length()) && (doc[p] == '_' ||
		(doc[p] >= 'a' && doc[p] <= 'z') ||
		(doc[p] >= 'A' && doc[p] <= 'Z') ||
		((unsigned char)doc[p]) > 177))
	{
		out_c = doc[p];
		return 1;
	}
	else if (p + 1 < doc.length() && doc[p] == '\\')
	{
		if ((doc[p + 1] >= '0' && doc[p + 1] <= '9') ||
			(doc[p + 1] >= 'a' && doc[p + 1] <= 'f') ||
			(doc[p + 1] >= 'A' && doc[p + 1] <= 'F'))
		{
			unsigned int unicode_char = 0;
			size_t p1 = p + 1;
			size_t p2 = p1;
			while (p2 < doc.length() && p2 - p1 < 6)
			{
				if (doc[p + 1] >= '0' && doc[p + 1] <= '9')
				{
					unicode_char <<= 4;
					unicode_char += (unsigned int)(doc[p + 1] - '0');
				}
				else if (doc[p + 1] >= 'a' && doc[p + 1] <= 'f')
				{
					unicode_char <<= 4;
					unicode_char += (unsigned int)(doc[p + 1] - 'a') + 10;
				}
				else if (doc[p + 1] >= 'A' && doc[p + 1] <= 'F')
				{
					unicode_char <<= 4;
					unicode_char += (unsigned int)(doc[p + 1] - 'A') + 10;
				}
				else
				{
					break;
				}
				p2++;
			}

			if (p2 + 1 < doc.length() && doc[p2] == '\r' && doc[p2 + 1] == '\n')
			{
				p2 += 2;
			}
			else if (p2 < doc.length() && (doc[p2] == '\r' || doc[p2] == '\n' || doc[p2] == '\t' || doc[p2] == '\f'))
			{
				p2++;
			}

			// To do: return the char as utf-8
			out_c = (std::string::value_type)unicode_char;
			return p2 - p1;
		}
		else if (doc[p + 1] != '\r' &&
			doc[p + 1] != '\n' &&
			doc[p + 1] != '\t' &&
			doc[p + 1] != '\f')
		{
			out_c = doc[p + 1];
			return 2;
		}
	}

	return 0;
}

size_t CSSTokenizerImpl::read_nmchar(size_t p, std::string::value_type& out_c)
{
	if (p + 1 <= doc.length())
	{
		out_c = doc[p];
		if (doc[p] >= '0' && doc[p] <= '9')
			return 1;
		else if (doc[p] == '-')
			return 1;
		else
			return read_nmstart(p, out_c);
	}
	else
	{
		return 0;
	}
}

bool CSSTokenizerImpl::is_whitespace(std::string::value_type c)
{
	switch (c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
	case '\f':
		return true;
	default:
		return false;
	}
}

size_t CSSTokenizerImpl::read_string(size_t p, std::string& out_str, std::string::value_type str_char) const
{
	out_str.clear();
	size_t end_pos = p;
	while (end_pos < doc.length() && doc[end_pos] != str_char)
	{
		if (doc[end_pos] == '\\')
		{
			if (end_pos + 2 < doc.length() && doc[end_pos + 1] == '\r' && doc[end_pos + 2] == '\n')
			{
				out_str.append(1, '\n');
				end_pos += 3;
			}
			else if (doc[end_pos + 1] == '\n' || doc[end_pos + 1] == '\f')
			{
				out_str.append(1, '\n');
				end_pos += 2;
			}
			else
			{
				// escape
				if ((doc[end_pos + 1] >= 'a' && doc[end_pos + 1] <= 'z') ||
					(doc[end_pos + 1] >= 'A' && doc[end_pos + 1] <= 'Z') ||
					(doc[end_pos + 1] >= '0' && doc[end_pos + 1] <= '9'))
				{
					size_t i;
					unsigned int character = 0;
					for (i = end_pos + 1; i < doc.length(); i++)
					{
						int v = 0;
						if ((doc[i] >= 'a' && doc[i] <= 'z'))
						{
							v = 10 + (doc[i] - 'a');
						}
						else if (doc[i] >= 'A' && doc[i] <= 'Z')
						{
							v = 10 + (doc[i] - 'A');
						}
						else if (doc[i] >= '0' && doc[i] <= '9')
						{
							v = doc[i] - '0';
						}
						else if (doc[i] == ' ' || doc[i] == '\t')
						{
							i++;
							break;
						}
						else
						{
							break;
						}
						character <<= 4;
						character += v;
					}
					if (character == 0)
						character = 0xfffd;
					out_str.append(1, character);
					end_pos = i;
				}
				else
				{
					out_str.append(1, doc[end_pos + 1]);
					end_pos += 2;
				}
			}
		}
		else if (end_pos + 1 < doc.length() && doc[end_pos] == '\r' && doc[end_pos + 1] == '\n')
		{
			out_str.clear();
			return p;
		}
		else if (doc[end_pos] == '\n')
		{
			out_str.clear();
			return p;
		}
		else
		{
			out_str.append(1, doc[end_pos]);
			end_pos++;
		}
	}
	if (end_pos == doc.length())
	{
		out_str.clear();
		return p;
	}
	return end_pos + 1;
}

size_t CSSTokenizerImpl::read_invalid(size_t p) const
{
	size_t end_pos = p;
	while (end_pos < doc.length() && doc[end_pos] != '\n')
		end_pos++;
	size_t doc_length = doc.length();
	return std::min((end_pos + 1), doc_length);
}

size_t CSSTokenizerImpl::read_uri_nonquoted_string(size_t p, std::string& out_str) const
{
	out_str.clear();
	size_t end_pos = p;
	while (end_pos < doc.length() && doc[end_pos] != ' ' && doc[end_pos] != '\t' && doc[end_pos] != ')')
	{
		if (doc[end_pos] == '\\')
		{
			if (end_pos + 2 < doc.length() && doc[end_pos + 1] == '\r' && doc[end_pos + 2] == '\n')
			{
				out_str.append(1, '\n');
				end_pos += 3;
			}
			else if (doc[end_pos + 1] == '\n')
			{
				out_str.append(1, '\n');
				end_pos += 2;
			}
			else
			{
				out_str.append(1, '?');
				// escape
				end_pos += 2;
			}
		}
		else if (end_pos + 1 < doc.length() && doc[end_pos] == '\r' && doc[end_pos + 1] == '\n')
		{
			out_str.clear();
			return p;
		}
		else if (doc[end_pos] == '\n')
		{
			out_str.clear();
			return p;
		}
		else
		{
			out_str.append(1, doc[end_pos]);
			end_pos++;
		}
	}
	if (end_pos == doc.length())
	{
		out_str.clear();
		return p;
	}
	return end_pos;
}

bool CSSTokenizerImpl::equal_caseless(const std::string& a, const std::string& b)
{
#ifdef WIN32
	return _stricmp(a.c_str(), b.c_str()) == 0;
#else
	return strcasecmp(a.c_str(), b.c_str()) == 0;
#endif
}
