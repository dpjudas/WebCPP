
#pragma once

#include "xml_tokenizer.h"

class XmlTokenizerImpl : public XmlTokenizer
{
public:
	XmlTokenizerImpl(const std::string& data);

	bool eat_whitespace() const override;
	void set_eat_whitespace(bool enable = true) override;
	void next(XmlToken *out_token) override;

private:
	std::string::size_type pos = 0, size = 0;
	std::string data;
	bool _eat_whitespace = true;

	static void throw_exception(const std::string &str);
	bool next_text_node(XmlToken *out_token);
	bool next_tag_node(XmlToken *out_token);
	bool next_exclamation_mark_node(XmlToken *out_token);

	// used to get the line number when there is an error in the xml file
	int get_line_number();

	void unescape(std::string &text_out, const std::string &text_in);
	void unescape(std::string &text, const std::string &search, std::string::value_type replace);
	std::string trim_whitespace(const std::string &text);
};
