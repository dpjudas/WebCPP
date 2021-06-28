
#pragma once

#include "xml_writer.h"
#include <string>

class XmlWriterImpl : public XmlWriter
{
public:
	XmlWriterImpl(std::function<void(const void* data, size_t size)> writeCallback);

	bool insert_whitespace() const override;
	void set_insert_whitespace(bool enable) override;
	void write(const XmlToken &token) override;

private:
	std::function<void(const void* data, size_t size)> writeCallback;
	bool _insert_whitespace = true;
	int indent = 0;
	std::string str;
	std::string escaped_string;
	bool first_token = true;
	bool single_line_tag = true;

	std::string insert_escapes_fast(const std::string &str);
};
