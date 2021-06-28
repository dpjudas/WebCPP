
#pragma once

#include "block_allocator.h"
#include "xml_document_impl.h"

class XmlDocumentImpl;

class XmlTreeNode : public BlockAllocated
{
public:
	std::string node_name;
	std::string node_value;
	std::string namespace_uri;
	XmlNodeType node_type = XmlNodeType::element;
	unsigned int parent = cl_null_node_index;
	unsigned int first_child = cl_null_node_index;
	unsigned int last_child = cl_null_node_index;
	unsigned int previous_sibling = cl_null_node_index;
	unsigned int next_sibling = cl_null_node_index;
	unsigned int first_attribute = cl_null_node_index;
	std::weak_ptr<XmlNodeImpl> public_node;

	void reset()
	{
		node_name.clear();
		node_value.clear();
		namespace_uri.clear();
		node_type = XmlNodeType::element;
		parent = cl_null_node_index;
		first_child = cl_null_node_index;
		last_child = cl_null_node_index;
		previous_sibling = cl_null_node_index;
		next_sibling = cl_null_node_index;
		first_attribute = cl_null_node_index;
	}

	std::string get_node_name() const
	{
		return node_name;
	}

	std::string get_node_value() const
	{
		return node_value;
	}

	std::string get_namespace_uri() const
	{
		return namespace_uri;
	}

	void set_node_name(XmlDocumentImpl *owner_document, const XmlString &str)
	{
		node_name = str;
	}

	void set_node_value(XmlDocumentImpl *owner_document, const XmlString &str)
	{
		node_value = str;
	}

	void set_namespace_uri(XmlDocumentImpl *owner_document, const XmlString &str)
	{
		namespace_uri = str;
	}

	XmlTreeNode *get_parent(XmlDocumentImpl *owner_document)
	{
		return parent != cl_null_node_index ? owner_document->nodes[parent] : nullptr;
	}

	const XmlTreeNode *get_parent(XmlDocumentImpl *owner_document) const
	{
		return parent != cl_null_node_index ? owner_document->nodes[parent] : nullptr;
	}

	XmlTreeNode *get_first_child(XmlDocumentImpl *owner_document)
	{
		return first_child != cl_null_node_index ? owner_document->nodes[first_child] : nullptr;
	}

	const XmlTreeNode *get_first_child(XmlDocumentImpl *owner_document) const
	{
		return first_child != cl_null_node_index ? owner_document->nodes[first_child] : nullptr;
	}

	XmlTreeNode *get_last_child(XmlDocumentImpl *owner_document)
	{
		return last_child != cl_null_node_index ? owner_document->nodes[last_child] : nullptr;
	}

	const XmlTreeNode *get_last_child(XmlDocumentImpl *owner_document) const
	{
		return last_child != cl_null_node_index ? owner_document->nodes[last_child] : nullptr;
	}

	XmlTreeNode *get_previous_sibling(XmlDocumentImpl *owner_document)
	{
		return previous_sibling != cl_null_node_index ? owner_document->nodes[previous_sibling] : nullptr;
	}

	const XmlTreeNode *get_previous_sibling(XmlDocumentImpl *owner_document) const
	{
		return previous_sibling != cl_null_node_index ? owner_document->nodes[previous_sibling] : nullptr;
	}

	XmlTreeNode *get_next_sibling(XmlDocumentImpl *owner_document)
	{
		return next_sibling != cl_null_node_index ? owner_document->nodes[next_sibling] : nullptr;
	}

	const XmlTreeNode *get_next_sibling(XmlDocumentImpl *owner_document) const
	{
		return next_sibling != cl_null_node_index ? owner_document->nodes[next_sibling] : nullptr;
	}

	XmlTreeNode *get_first_attribute(XmlDocumentImpl *owner_document)
	{
		return first_attribute != cl_null_node_index ? owner_document->nodes[first_attribute] : nullptr;
	}

	const XmlTreeNode *get_first_attribute(XmlDocumentImpl *owner_document) const
	{
		return first_attribute != cl_null_node_index ? owner_document->nodes[first_attribute] : nullptr;
	}
};
