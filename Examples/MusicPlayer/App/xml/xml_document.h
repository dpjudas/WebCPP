
#pragma once

#include "xml_node.h"
#include <vector>
#include <functional>

/// XML document tree
class XmlDocument : public XmlNode
{
public:
	static std::shared_ptr<XmlDocument> create();
	static std::shared_ptr<XmlDocument> load(const std::string& input, bool eat_whitespace = true);

	virtual std::shared_ptr<XmlNode> document_element() const = 0;

	virtual std::shared_ptr<XmlNode> create_element(const XmlString &tag_name) = 0;
	virtual std::shared_ptr<XmlNode> create_element(const XmlString &namespace_uri, const XmlString &qualified_name) = 0;
	virtual std::shared_ptr<XmlNode> create_fragment() = 0;
	virtual std::shared_ptr<XmlNode> create_text(const XmlString &text) = 0;
	virtual std::shared_ptr<XmlNode> create_comment(const XmlString &data) = 0;
	virtual std::shared_ptr<XmlNode> create_cdata(const XmlString &data) = 0;
	virtual std::shared_ptr<XmlNode> create_attribute(const XmlString &name) = 0;
	virtual std::shared_ptr<XmlNode> create_attribute(const XmlString &namespace_uri, const XmlString &qualified_name) = 0;

	/// \brief Imports a node from another document to this document.
	/// The returned node has no parent. The source node is not altered or removed from the original document; this method creates a new copy of the source node.
	virtual std::shared_ptr<XmlNode> import_node(const std::shared_ptr<XmlNode> &node, bool deep) = 0;

	virtual void save(std::function<void(const void* data, size_t size)> writeCallback, bool insert_whitespace = true) const = 0;
};
