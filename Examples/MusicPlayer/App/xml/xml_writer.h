
#pragma once

#include <memory>
#include <functional>

class XmlToken;

/// \brief The XML Writer writes a XML file based on XML tokens.
class XmlWriter
{
public:
	/// \brief Constructs a XmlWriter
	static std::shared_ptr<XmlWriter> create(std::function<void(const void *data, size_t size)> writeCallback);

	/// \brief Returns the insert whitespace flag.
	virtual bool insert_whitespace() const = 0;

	/// \brief Inserts whitespace between tags if enabled.
	virtual void set_insert_whitespace(bool enable = true) = 0;

	/// \brief Write token to file.
	virtual void write(const XmlToken &token) = 0;
};
