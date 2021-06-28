
#include "xml_node.h"
#include "xml_document.h"
#include "xml_node_impl.h"
#include "xml_document_impl.h"
#include "xml_tree_node.h"

XmlString XmlNodeImpl::name() const
{
	const XmlTreeNode *tree_node = get_tree_node();
	switch (tree_node->node_type)
	{
	case XmlNodeType::cdata:
		return "#cdata-section";
	case XmlNodeType::comment:
		return "#comment";
	case XmlNodeType::document:
		return "#document";
	case XmlNodeType::document_fragment:
		return "#document-fragment";
	case XmlNodeType::text:
		return "#text";
	case XmlNodeType::attribute:
	case XmlNodeType::element:
	default:
		return tree_node->get_node_name();
	}
}

XmlString XmlNodeImpl::text() const
{
	const XmlTreeNode *tree_node = get_tree_node();
	if (tree_node->node_type == XmlNodeType::element)
	{
		std::string str;
		std::shared_ptr<XmlNode> cur = first_child();
		while (cur)
		{
			if (cur->is_text() || cur->is_cdata_section() || cur->is_element())
				str.append(cur->text());
			cur = cur->next_sibling();
		}
		return str;
	}
	else
	{
		return tree_node->get_node_value();
	}
}

XmlString XmlNodeImpl::namespace_uri() const
{
	return get_tree_node()->get_namespace_uri();
}

XmlString XmlNodeImpl::prefix() const
{
	XmlString node_name = get_tree_node()->get_node_name();
	XmlString::size_type pos = node_name.find(':');
	if (pos != XmlString::npos)
		return node_name.substr(0, pos);
	else
		return XmlString();
}

void XmlNodeImpl::set_prefix(const XmlString &prefix)
{
	XmlString node_name = get_tree_node()->get_node_name();
	XmlString::size_type pos = node_name.find(':');
	if (pos == XmlString::npos)
		get_tree_node()->set_node_name(_owner_document.get(), prefix + ':' + node_name);
	else
		get_tree_node()->set_node_name(_owner_document.get(), prefix + node_name.substr(pos));
}

XmlString XmlNodeImpl::local_name() const
{
	XmlString node_name = get_tree_node()->get_node_name();
	XmlString::size_type pos = node_name.find(':');
	if (pos != XmlString::npos)
		return node_name.substr(pos + 1);
	else
		return node_name;
}

void XmlNodeImpl::set_text(const XmlString &value)
{
	if (is_element())
	{
		bool complex_children = first_child() != last_child() || (first_child() && !first_child()->is_text());
		if (complex_children)
		{
			while (last_child())
				remove_child(last_child());
		}

		if (first_child())
		{
			first_child()->set_text(value);
		}
		else
		{
			append_child(owner_document()->create_text(value));
		}
	}
	else
	{
		auto tree_node = get_tree_node();
		tree_node->set_node_value(_owner_document.get(), value);
	}
}

XmlNodeType XmlNodeImpl::type() const
{
	return get_tree_node()->node_type;
}

std::shared_ptr<XmlNode> XmlNodeImpl::parent() const
{
	const XmlTreeNode *tree_node = get_tree_node();
	if (tree_node->parent != cl_null_node_index)
	{
		if (tree_node->parent != _owner_document->node_index)
			return _owner_document->allocate_dom_node(tree_node->parent);
		else
			return _owner_document;
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::first_child() const
{
	const XmlTreeNode *tree_node = get_tree_node();
	if (tree_node->first_child != cl_null_node_index)
	{
		return _owner_document->allocate_dom_node(tree_node->first_child);
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::last_child() const
{
	const XmlTreeNode *tree_node = get_tree_node();
	if (tree_node->last_child != cl_null_node_index)
	{
		return _owner_document->allocate_dom_node(tree_node->last_child);
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::first_attribute() const
{
	const XmlTreeNode *tree_node = get_tree_node();
	if (tree_node->first_attribute != cl_null_node_index)
	{
		return _owner_document->allocate_dom_node(tree_node->first_attribute);
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::previous_sibling() const
{
	const XmlTreeNode *tree_node = get_tree_node();
	if (tree_node->previous_sibling != cl_null_node_index)
	{
		return _owner_document->allocate_dom_node(tree_node->previous_sibling);
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::next_sibling() const
{
	const XmlTreeNode *tree_node = get_tree_node();
	if (tree_node->next_sibling != cl_null_node_index)
	{
		return _owner_document->allocate_dom_node(tree_node->next_sibling);
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlDocument> XmlNodeImpl::owner_document() const
{
	return _owner_document;
}

bool XmlNodeImpl::has_attributes() const
{
	return get_tree_node()->first_attribute != cl_null_node_index;
}

std::shared_ptr<XmlNode> XmlNodeImpl::insert_before(const std::shared_ptr<XmlNode> &new_child, const std::shared_ptr<XmlNode> &ref_child)
{
	if (!ref_child)
	{
		append_child(new_child);
		return new_child;
	}
	else if (new_child && ref_child)
	{
		if (new_child->parent())
			new_child->parent()->remove_child(new_child);

		auto new_child_impl = static_cast<XmlNodeImpl*>(new_child.get());
		auto ref_child_impl = static_cast<XmlNodeImpl*>(ref_child.get());
		XmlTreeNode *tree_node = get_tree_node();
		XmlTreeNode *new_tree_node = new_child_impl->get_tree_node();
		XmlTreeNode *ref_tree_node = ref_child_impl->get_tree_node();

		new_tree_node->previous_sibling = ref_tree_node->previous_sibling;
		new_tree_node->next_sibling = ref_child_impl->node_index;
		ref_tree_node->previous_sibling = new_child_impl->node_index;
		if (new_tree_node->previous_sibling != cl_null_node_index)
			new_tree_node->get_previous_sibling(_owner_document.get())->next_sibling = new_child_impl->node_index;
		if (tree_node->first_child == ref_child_impl->node_index)
			tree_node->first_child = new_child_impl->node_index;
		new_tree_node->parent = node_index;

		return new_child;
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::replace_child(const std::shared_ptr<XmlNode> &new_child, const std::shared_ptr<XmlNode> &old_child)
{
	if (new_child && old_child)
	{
		if (new_child->parent())
			new_child->parent()->remove_child(new_child);

		auto new_child_impl = static_cast<XmlNodeImpl*>(new_child.get());
		auto old_child_impl = static_cast<XmlNodeImpl*>(old_child.get());

		XmlTreeNode *tree_node = get_tree_node();
		XmlTreeNode *new_tree_node = new_child_impl->get_tree_node();
		XmlTreeNode *old_tree_node = old_child_impl->get_tree_node();

		new_tree_node->previous_sibling = old_tree_node->previous_sibling;
		new_tree_node->next_sibling = old_tree_node->next_sibling;
		new_tree_node->parent = node_index;
		if (tree_node->first_child == old_child_impl->node_index)
			tree_node->first_child = new_child_impl->node_index;
		if (tree_node->last_child == old_child_impl->node_index)
			tree_node->last_child = new_child_impl->node_index;
		old_tree_node->previous_sibling = cl_null_node_index;
		old_tree_node->next_sibling = cl_null_node_index;
		old_tree_node->parent = cl_null_node_index;

		return new_child;
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::remove_child(const std::shared_ptr<XmlNode> &old_child)
{
	if (old_child)
	{
		auto old_child_impl = static_cast<XmlNodeImpl*>(old_child.get());
		XmlTreeNode *tree_node = get_tree_node();
		XmlTreeNode *old_tree_node = old_child_impl->get_tree_node();
		unsigned int prev_index = old_tree_node->previous_sibling;
		unsigned int next_index = old_tree_node->next_sibling;
		XmlTreeNode *prev = old_tree_node->get_previous_sibling(_owner_document.get());
		XmlTreeNode *next = old_tree_node->get_next_sibling(_owner_document.get());
		if (next)
			next->previous_sibling = prev_index;
		if (prev)
			prev->next_sibling = next_index;
		if (tree_node->first_child == old_child_impl->node_index)
			tree_node->first_child = next_index;
		if (tree_node->last_child == old_child_impl->node_index)
			tree_node->last_child = prev_index;
		old_tree_node->previous_sibling = cl_null_node_index;
		old_tree_node->next_sibling = cl_null_node_index;
		old_tree_node->parent = cl_null_node_index;
		return old_child;
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::append_child(const std::shared_ptr<XmlNode> &new_child)
{
	if (new_child)
	{
		if (new_child->parent())
			new_child->parent()->remove_child(new_child);

		auto new_child_impl = static_cast<XmlNodeImpl*>(new_child.get());
		XmlTreeNode *tree_node = get_tree_node();
		XmlTreeNode *new_tree_node = new_child_impl->get_tree_node();
		if (tree_node->last_child != cl_null_node_index)
		{
			XmlTreeNode *last_tree_node = tree_node->get_last_child(_owner_document.get());
			last_tree_node->next_sibling = new_child_impl->node_index;
			new_tree_node->previous_sibling = tree_node->last_child;
			tree_node->last_child = new_child_impl->node_index;
		}
		else
		{
			tree_node->first_child = new_child_impl->node_index;
			tree_node->last_child = new_child_impl->node_index;
		}
		new_tree_node->parent = node_index;
		return new_child;
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::remove_attribute(const std::shared_ptr<XmlNode> &old_child)
{
	if (old_child)
	{
		auto old_child_impl = static_cast<XmlNodeImpl*>(old_child.get());
		XmlTreeNode *tree_node = get_tree_node();
		XmlTreeNode *old_tree_node = old_child_impl->get_tree_node();
		unsigned int prev_index = old_tree_node->previous_sibling;
		unsigned int next_index = old_tree_node->next_sibling;
		XmlTreeNode *prev = old_tree_node->get_previous_sibling(_owner_document.get());
		XmlTreeNode *next = old_tree_node->get_next_sibling(_owner_document.get());
		if (next)
			next->previous_sibling = prev_index;
		if (prev)
			prev->next_sibling = next_index;
		if (tree_node->first_attribute == old_child_impl->node_index)
			tree_node->first_attribute = next_index;
		old_tree_node->previous_sibling = cl_null_node_index;
		old_tree_node->next_sibling = cl_null_node_index;
		old_tree_node->parent = cl_null_node_index;
		return old_child;
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::add_attribute(const std::shared_ptr<XmlNode> &new_child)
{
	if (new_child)
	{
		auto new_child_impl = static_cast<XmlNodeImpl*>(new_child.get());
		XmlTreeNode *tree_node = get_tree_node();
		XmlTreeNode *new_tree_node = new_child_impl->get_tree_node();
		if (tree_node->first_attribute != cl_null_node_index)
		{
			XmlTreeNode *first_tree_node = tree_node->get_first_attribute(_owner_document.get());
			first_tree_node->previous_sibling = new_child_impl->node_index;
			new_tree_node->next_sibling = tree_node->first_attribute;
			tree_node->first_attribute = new_child_impl->node_index;
		}
		else
		{
			tree_node->first_attribute = new_child_impl->node_index;
		}
		new_tree_node->parent = node_index;
		return new_child;
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::clone(bool deep) const
{
	return owner_document()->import_node(const_cast<XmlNodeImpl*>(this)->shared_from_this(), deep);
}

std::shared_ptr<XmlNode> XmlNodeImpl::named_item(const XmlString &name) const
{
	auto node = first_child();
	while (node)
	{
		if (node->name() == name) return node;
		node = node->next_sibling();
	}
	return nullptr;
}

std::shared_ptr<XmlNode> XmlNodeImpl::named_item(const XmlString &namespace_uri, const XmlString &local_name) const
{
	auto node = first_child();
	while (node)
	{
		if (node->namespace_uri() == namespace_uri && node->local_name() == local_name)
			return node;
		node = node->next_sibling();
	}
	return nullptr;
}

bool XmlNodeImpl::has_attribute(const XmlString &name) const
{
	const XmlTreeNode *tree_node = get_tree_node();
	unsigned int cur_index = tree_node->first_attribute;
	const XmlTreeNode *cur_attribute = tree_node->get_first_attribute(_owner_document.get());
	while (cur_attribute)
	{
		if (cur_attribute->get_node_name() == name)
			return true;

		cur_index = cur_attribute->next_sibling;
		cur_attribute = cur_attribute->get_next_sibling(_owner_document.get());
	}
	return false;
}

bool XmlNodeImpl::has_attribute(const XmlString &namespace_uri, const XmlString &local_name) const
{
	for (auto attr = first_attribute(); attr; attr = attr->next_sibling())
	{
		if (attr->local_name() == local_name && attr->namespace_uri() == namespace_uri)
			return true;
	}
	return false;
}

XmlString XmlNodeImpl::attribute(const XmlString &name, const XmlString &default_value) const
{
	const XmlTreeNode *tree_node = get_tree_node();
	unsigned int cur_index = tree_node->first_attribute;
	const XmlTreeNode *cur_attribute = tree_node->get_first_attribute(_owner_document.get());
	while (cur_attribute)
	{
		if (cur_attribute->get_node_name() == name)
			return cur_attribute->get_node_value();

		cur_index = cur_attribute->next_sibling;
		cur_attribute = cur_attribute->get_next_sibling(_owner_document.get());
	}
	return default_value;
}

XmlString XmlNodeImpl::attribute_ns(const XmlString &namespace_uri, const XmlString &local_name, const XmlString &default_value) const
{
	for (auto attr = first_attribute(); attr; attr = attr->next_sibling())
	{
		if (attr->local_name() == local_name && attr->namespace_uri() == namespace_uri)
			return attr->text();
	}
	return default_value;
}

void XmlNodeImpl::set_attribute(const XmlString &name, const XmlString &value)
{
	for (auto attr = first_attribute(); attr; attr = attr->next_sibling())
	{
		if (attr->name() == name)
		{
			attr->set_text(value);
			return;
		}
	}

	auto attr = owner_document()->create_attribute(name);
	attr->set_text(value);
	add_attribute(attr);
}

void XmlNodeImpl::set_attribute(const XmlString &namespace_uri, const XmlString &qualified_name, const XmlString &value)
{
	for (auto attr = first_attribute(); attr; attr = attr->next_sibling())
	{
		if (attr->name() == qualified_name && attr->namespace_uri() == namespace_uri)
		{
			attr->set_text(value);
			return;
		}
	}

	auto attr = owner_document()->create_attribute(namespace_uri, qualified_name);
	attr->set_text(value);
	add_attribute(attr);
}

void XmlNodeImpl::remove_attribute(const XmlString &name)
{
	for (auto attr = first_attribute(); attr; attr = attr->next_sibling())
	{
		if (attr->name() == name)
		{
			remove_attribute(attr);
			return;
		}
	}
}

void XmlNodeImpl::remove_attribute(const XmlString &namespace_uri, const XmlString &local_name)
{
	for (auto attr = first_attribute(); attr; attr = attr->next_sibling())
	{
		if (attr->local_name() == local_name && attr->namespace_uri() == namespace_uri)
		{
			remove_attribute(attr);
			return;
		}
	}
}

std::shared_ptr<XmlNode> XmlNodeImpl::attribute_node(const XmlString &name) const
{
	for (auto attr = first_attribute(); attr; attr = attr->next_sibling())
	{
		if (attr->name() == name)
			return attr;
	}
	return nullptr;
}

std::shared_ptr<XmlNode> XmlNodeImpl::attribute_node(const XmlString &namespace_uri, const XmlString &local_name) const
{
	for (auto attr = first_attribute(); attr; attr = attr->next_sibling())
	{
		if (attr->local_name() == local_name && attr->namespace_uri() == namespace_uri)
			return attr;
	}
	return nullptr;
}

XmlString XmlNodeImpl::find_namespace_uri(const XmlString &qualified_name) const
{
	static XmlString xmlns_prefix("xmlns:");
	static XmlString xmlns_xml("xml");
	static XmlString xmlns_xml_uri("http://www.w3.org/XML/1998/namespace");
	static XmlString xmlns_xmlns("xmlns");
	static XmlString xmlns_xmlns_uri("http://www.w3.org/2000/xmlns/");

	XmlString prefix;
	XmlString::size_type pos = qualified_name.find(':');
	if (pos != XmlString::npos)
		prefix = qualified_name.substr(0, pos);

	if (prefix == xmlns_xml)
		return xmlns_xml;
	else if (prefix == xmlns_xmlns || qualified_name == xmlns_xmlns)
		return xmlns_xmlns;

	const XmlTreeNode *cur = get_tree_node();
	while (cur)
	{
		const XmlTreeNode *cur_attr = cur->get_first_attribute(_owner_document.get());
		while (cur_attr)
		{
			std::string node_name = cur_attr->get_node_name();
			if (prefix.empty())
			{
				if (node_name == xmlns_xmlns)
					return cur_attr->get_node_value();
			}
			else
			{
				if (node_name.substr(0, 6) == xmlns_prefix && node_name.substr(6) == prefix)
					return cur_attr->get_node_value();
			}
			cur_attr = cur_attr->get_next_sibling(_owner_document.get());
		}
		cur = cur->get_parent(_owner_document.get());
	}
	return XmlString();
}

XmlString XmlNodeImpl::find_prefix(const XmlString &namespace_uri) const
{
	for (auto cur = shared_from_this(); cur; cur = cur->parent())
	{
		for (auto attr = cur->first_attribute(); attr; attr = attr->next_sibling())
		{
			if (attr->prefix() == "xmlns" && attr->text() == namespace_uri)
			{
				return attr->local_name();
			}
		}
	}
	return XmlString();
}

XmlTreeNode *XmlNodeImpl::get_tree_node()
{
	if (node_index == cl_null_node_index)
		return nullptr;
	return _owner_document->nodes[node_index];
}

const XmlTreeNode *XmlNodeImpl::get_tree_node() const
{
	if (node_index == cl_null_node_index)
		return nullptr;
	return _owner_document->nodes[node_index];
}
