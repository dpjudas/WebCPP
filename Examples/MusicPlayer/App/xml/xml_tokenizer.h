
#pragma once

#include "xml_token.h"
#include <memory>
#include <string>

class XmlToken;

/// \brief The XML Tokenizer breaks a XML file into XML tokens.
class XmlTokenizer
{
public:
	/// \brief Constructs a XmlTokenizer
	static std::shared_ptr<XmlTokenizer> create(const std::string& data);

	/// \brief Returns true if eat whitespace flag is set.
	virtual bool eat_whitespace() const = 0;

	/// \brief If enabled, will eat any whitespace between tags.
	virtual void set_eat_whitespace(bool enable = true) = 0;

	/// \brief Returns the next token available in input stream.
	XmlToken next() { XmlToken token; next(&token); return token; }
	virtual void next(XmlToken *out_token) = 0;
};
