
#pragma once

#include <vector>
#include <utility>
#include <string>

enum class XmlTokenType
{
	null,
	element,
	text,
	cdata,
	processing_instruction,
	comment,
	doctype
};

enum class XmlTokenVariant
{
	begin,
	end,
	single
};

/// \brief XML token in a XML file.
class XmlToken
{
public:
	/// Attribute name/value pair.
	typedef std::pair<std::string, std::string> Attribute;

	/// \brief The token type.
	XmlTokenType type = XmlTokenType::null;

	/// \brief The token variant.
	XmlTokenVariant variant = XmlTokenVariant::single;

	/// \brief The name of the token.
	std::string name;

	/// \brief Returns the value of the token.
	std::string value;

	/// \brief All the attributes attached to the token.
	std::vector<Attribute> attributes;
};
