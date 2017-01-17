  /* -------------------------------------------------------------------------
//	FileName	：	third_party/rapidxml/yx_xml.cc
//	Creator		：	(zc) <zcnet4@gmail.com>
//	CreateTime	：	2013-04-18 10:08:19
//	Description	：	
//
// -----------------------------------------------------------------------*/

#include "yx_xml.h"
#include <stdio.h>
#include <string.h>
//#include "base/strings/string_util.h"


#ifdef YG_USE_TINYXML
	#include "tinyxml.h"		// for tinyxml
  #define refXmlDoc   (xml_node_->GetDocument())
#else
  #define RAPIDXML_STATIC_POOL_SIZE (8 * 1024)
  #include "rapidxml/rapidxml.hpp"
  #include "rapidxml/rapidxml_print.hpp"
  typedef rapidxml::node_type RdXmlNodeType;

  #define refXmlDoc   (xml_node_->document())
  #define doc_ptr() (xml_attribute_->document())
  
  // 判断node是否一个elment
  inline bool IsElement(RdXmlNode* node) {
    return node && node->type() == rapidxml::node_element;
  }
#endif // _DEBUG

#include "string_util.h"
inline int _StrCaseCmp(const char* s1, const char* s2) {
	return yx::strcasecmp(s1, s2);
}

char* allocate_string(RdXmlDocument& doc, const char* str, size_t sz = 0)
{
  char*new_str = nullptr;
  if (sz > 0)
  {
    // 多分配一个字符
    new_str = doc.allocate_string(nullptr, sz + 1);
    memcpy(new_str, str, sizeof(char) * sz);
    new_str[sz] = 0;
  } else if (sz == 0) {
    new_str = doc.allocate_string(str, 0);
  }
  //
  return new_str;
}
//YYBrowser Begin
//by ZC. 2014-3-27 16:04.
#ifndef IsValidString
  #define IsValidString(x) static_cast<bool>(x && x[0])
#endif
//YYBrowser End

// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// HMXmlAttribute
XmlAttribute::XmlAttribute()
	: xml_attribute_(nullptr)
{
}

XmlAttribute::XmlAttribute(XmlAttribute&& other)
  : xml_attribute_(other.xml_attribute_)
{
  other.xml_attribute_ = nullptr;
}

XmlAttribute& XmlAttribute::operator=(XmlAttribute&& other)
{
  // other为右值，不会等于this
  xml_attribute_ = other.xml_attribute_;
  other.xml_attribute_ = nullptr;
  return *this;
}
/*
@func		: isValid
@brief		: 是否可用。
*/
bool XmlAttribute::isValid() const {
	return (xml_attribute_ != NULL);
}

/*
@func		: isEqual
@brief		: 是否相同。
*/
bool XmlAttribute::isEqual(const XmlAttribute& other) const {
	return (xml_attribute_ == other.xml_attribute_);
}

/*
@func		: Name
@brief		: 
*/
const char*	XmlAttribute::getName() const {
	if (xml_attribute_ != NULL) {
#ifdef YG_USE_TINYXML
		return xml_attribute_->Name();
#else
		return xml_attribute_->name();
#endif
	}
	return NULL;
}

/*
@func		: Value
@brief		: 
*/
const char*	XmlAttribute::getValue() const {
	if (xml_attribute_ != NULL) {
#ifdef YG_USE_TINYXML
		return xml_attribute_->Value();
#else
		return xml_attribute_->value();
#endif
	}
	return NULL;
}

/*
@func		: Name
@brief		: 
*/
void XmlAttribute::setName(const char* str, size_t sz) const
{
  if (isValid() && IsValidString(str))
  {
#ifdef YG_USE_TINYXML
#error "not implemented"
#else
    RdXmlDocument* doc = doc_ptr();
    char* str_value = allocate_string(*doc, str, sz);
		xml_attribute_->name(str_value, sz);
#endif
  }
}
/*
@func		: Value
@brief		: 
*/
void XmlAttribute::setValue(const char* str, size_t sz) const
{
  if (isValid())
  {
#ifdef YG_USE_TINYXML
#error "not implemented"
#else
    RdXmlDocument* doc = doc_ptr();
    if (IsValidString(str))
    {
      const char* str_value = allocate_string(*doc, str, sz);
      xml_attribute_->value(str_value, sz);
    } else {
      xml_attribute_->value(nullptr, 0);
    }
#endif
  }
}
/*
@func		: Next
@brief		: 
*/
XmlAttribute XmlAttribute::nextAttribute() const {
	XmlAttribute xmlAttribute;
	if (xml_attribute_ != NULL) {
#ifdef YG_USE_TINYXML
		xmlAttribute.xml_attribute_ = xml_attribute_->Next();
#else
		xmlAttribute.xml_attribute_ = xml_attribute_->next_attribute();
#endif
	}
	return xmlAttribute;
}

//////////////////////////////////////////////////////////////////////////
// static
RdXmlNodeType toRdXmlNodeType(XmlNode::Type node_type)
{
  RdXmlNodeType local_type;
#ifdef YG_USE_TINYXML
  #error "not implemented"
#else
  switch (node_type)
  {
    case XmlNode::Type::Document:
      local_type = rapidxml::node_document;
      break;
    case XmlNode::Type::Element:
      local_type = rapidxml::node_element;
      break;
    case XmlNode::Type::Data:
      local_type = rapidxml::node_data;
      break;
    case XmlNode::Type::CData:
      local_type = rapidxml::node_cdata;
      break;
    case XmlNode::Type::Comment:
      local_type = rapidxml::node_comment;
      break;
    case XmlNode::Type::Declaration:
      local_type = rapidxml::node_declaration;
      break;
    case XmlNode::Type::DocType:
      local_type = rapidxml::node_doctype;
      break;
    case XmlNode::Type::PI:
      local_type = rapidxml::node_pi;
      break;
    default:
      assert(false);
      local_type = rapidxml::node_element;
      break;
  }
#endif
  return local_type;
}

//////////////////////////////////////////////////////////////////////////
// HMXmlNode
XmlNode::XmlNode()
	: xml_node_(nullptr) 
{
}

XmlNode::XmlNode(XmlNode&& other)
  : xml_node_(other.xml_node_)
{
  other.xml_node_ = nullptr;
}

XmlNode& XmlNode::operator=(XmlNode&& other)
{
  xml_node_ = other.xml_node_;
  other.xml_node_ = nullptr;
  return *this;
}
const char* XmlNode::getName() const
{
#ifdef YG_USE_TINYXML
#error "not implemented"
#else
  return isValid() ?  xml_node_->name() : nullptr;
#endif
}

void XmlNode::setName(const char* name, size_t sz)
{
#ifdef YG_USE_TINYXML
#error "not implemented"
#else
  if (isValid() && IsValidString(name))
  {
    RdXmlDocument* doc = refXmlDoc;
    const char* name_copy = allocate_string(*doc, name, sz);
    xml_node_->name(name_copy, sz);
  }
#endif
}

const char* XmlNode::getValue() const
{
#ifdef YG_USE_TINYXML
#error "not implemented"
#else
  return isValid() ?  xml_node_->value() : nullptr;
#endif
}

void XmlNode::setValue(const char* name, size_t sz)
{
#ifdef YG_USE_TINYXML
#error "not implemented"
#else
  if (isValid() && IsValidString(name))
  {
    RdXmlDocument* doc = refXmlDoc;
    const char* name_copy = allocate_string(*doc, name, sz);
    xml_node_->value(name_copy, sz);
  }
#endif
}
/*
@func		: isValid
@brief		: 是否可用。
*/
bool XmlNode::isValid() const {
	return (xml_node_ != NULL);
}

/*
@func		: isEqual
@brief		: 是否相同。
*/
bool XmlNode::isEqual(const XmlNode& other) const {
	return (xml_node_ == other.xml_node_);
}

/*
@func		: isEqual
@brief		: 是否相同。
*/
bool XmlNode::isType(Type test_type) const
{
  return isValid() && (xml_node_->type() == toRdXmlNodeType(test_type));
}
/*
@func		: firstChild
@brief		:
*/
XmlNode XmlNode::firstChild(const char* xmlTag/* = NULL*/) const {
	XmlNode xmlNodeObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlNodeObj.xml_node_ = xml_node_->FirstChild(xmlTag);
		} else {
			xmlNodeObj.xml_node_ = xml_node_->FirstChild();
		}
#else
		xmlNodeObj.xml_node_	= xml_node_->first_node(xmlTag);
#endif // _DEBUG
		
	}
	return xmlNodeObj;
}

/*
@func		: lastChild
@brief		:
*/
XmlNode XmlNode::lastChild(const char* xmlTag/* = NULL*/) const {
	XmlNode xmlNodeObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlNodeObj.xml_node_ = xml_node_->LastChild(xmlTag);
		} else {
			xmlNodeObj.xml_node_ = xml_node_->LastChild();
		}
#else
		xmlNodeObj.xml_node_	= xml_node_->last_node(xmlTag);
#endif
	}
	return xmlNodeObj;
}

/*
@func		: previousSibling
@brief		:
*/
XmlNode XmlNode::previousSibling(const char* xmlTag/* = NULL*/) const {
	XmlNode xmlNodeObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlNodeObj.xml_node_ = xml_node_->PreviousSibling(xmlTag);
		} else {
			xmlNodeObj.xml_node_ = xml_node_->PreviousSibling();
		}
#else
		xmlNodeObj.xml_node_	= xml_node_->previous_sibling(xmlTag);
#endif
	}
	return xmlNodeObj;
}

/*
@func		: nextSibling
@brief		: Navigate to a sibling node.
*/
XmlNode XmlNode::nextSibling(const char* xmlTag/* = NULL*/) const {
	XmlNode xmlNodeObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlNodeObj.xml_node_ = xml_node_->NextSibling(xmlTag);
		} else {
			xmlNodeObj.xml_node_ = xml_node_->NextSibling();
		}
#else
		xmlNodeObj.xml_node_	= xml_node_->next_sibling(xmlTag);
#endif
	}
	return xmlNodeObj;
}

/*
@func			: document
@brief		:
*/
XmlDocumentBase XmlNode::document() {
  XmlDocumentBase xmlDoc;
  if (isValid()) 
  {
    xmlDoc.xml_doc_ = refXmlDoc;
  }
  //
  return xmlDoc;
}

/*
@func			: print
@brief		:
*/
void XmlNode::print(std::string& out) {
  if (xml_node_)
    rapidxml::print(std::back_inserter(out), *xml_node_, 0);
}

/*
@func			: parent
@brief		: 返回父节点
*/
XmlNode XmlNode::parent() {
  XmlNode xmlNodeObj;
#ifdef YG_USE_TINYXML
    xmlNodeObj.xml_node_ = xml_node_->parent();
#else
		xmlNodeObj.xml_node_	= xml_node_->parent();
#endif
  return xmlNodeObj;
}

/*
@func			: shallowCopy
@brief		: 把当前的节点树做一份拷贝，但属性值，name,value都跟当前节点共用
            一份存储。
*/
XmlNode XmlNode::shallowCopy()
{
  return document().shallowCopy(*this);
}
///////////////////////////////////////////////////////////////////////////
// Node manipulation
//! Prepends a new child node.
//! The prepended child becomes the first child, and all existing children are moved one position back.
//! \param child Node to prepend.
void XmlNode::prependNode(const XmlNode& node) {
  if (!node.isValid()) {
    return;
  }
  // 先从旧的父节点移除.
  if (node.xml_node_->parent()) {
    node.xml_node_->parent()->remove_node(node.xml_node_);
  }
  xml_node_->prepend_node(node.xml_node_);
}
//! Appends a new child node. 
//! The appended child becomes the last child.
//! \param child Node to append.
void XmlNode::appendNode(const XmlNode& node) {
  if (!node.isValid()) {
    return;
  }
  // 先从旧的父节点移除.
  if (node.xml_node_->parent()) {
    node.xml_node_->parent()->remove_node(node.xml_node_);
  }
  // 加入到新的父节点下
  xml_node_->append_node(node.xml_node_);
}
//! Inserts a new child node at specified place inside the node. 
//! All children after and including the specified node are moved one position back.
//! \param where Place where to insert the child, or 0 to insert at the back.
//! \param child Node to insert.
void XmlNode::insertNode(const XmlNode& where, const XmlNode& child) {
  if (!child.isValid() || 
      (where.isValid() && where.xml_node_->parent() != xml_node_) ||
      child.xml_node_->type() == rapidxml::node_document ||
      child.xml_node_ == where.xml_node_) {
    return;
  }

  // 先从旧父节点移除.
  if (child.xml_node_->parent()) {
    child.xml_node_->parent()->remove_node(child.xml_node_);
  }
  // 插入到新的节点.
  xml_node_->insert_node(where.xml_node_, child.xml_node_);
}
//! Removes specified child from the node
// \param child Pointer to child to be removed.

XmlNode XmlNode::removeNode(const XmlNode& child) {
  XmlNode next_node;
  // 的确是自己的子节点.
  if (child.isValid() && child.xml_node_->parent() == xml_node_) {
    next_node = child.nextSibling();
    xml_node_->remove_node(child.xml_node_);
  }
  return next_node;
}

// remove all child nodes of $node.
void XmlNode::removeAllChildren() {
  assert(isValid());
  xml_node_->remove_all_nodes();
}

void XmlNode::removeAllAttributes() {
  assert(isValid());
  xml_node_->remove_all_attributes();
}

//////////////////////////////////////////////////////////////////////////
// HMXmlElement
/*
@func		: tagName
@brief		: 如果当前节点是element，则返回其标签名。
*/
const char* XmlElement::tagName() const {
#ifdef YG_USE_TINYXML
	if(xml_node_ && NULL != xml_node_->ToElement()) {
		return xml_node_->Value();
	}
#else
	if (IsElement(xml_node_)) {
		return xml_node_->name();
	}
#endif // YG_USE_TINYXML
	return NULL;
}

/*
@func		: isTagName
@brief		: 是否是指定的标签名。
*/
bool XmlElement::isTagName(const char* pszTag) {
	const char* xmlTagName = tagName();
	if (xmlTagName && xmlTagName[0] != '\0' && pszTag) {
    return _StrCaseCmp(xmlTagName, pszTag) == 0;
	}
	
	return false;
}

/*
@func		: getText
@brief		: 如果当前节点含有TiXmlText节点，则返回相应的值，否则，返回NULL
			  宽字符版本则返回“”字符串
*/
const char* XmlElement::getText() const {
#ifdef YG_USE_TINYXML
	if (xml_node_) {
		TiXmlElement* xmlElem = xml_node_->ToElement();
		if (xmlElem) {
			return xmlElem->GetText();
		}
	}
#else
	if (IsElement(xml_node_)) {
		// ASSERT(FALSE);
		RdXmlNode* pChildNode = xml_node_->first_node();
		if (pChildNode 
			&& (pChildNode->type() == rapidxml::node_data 
			    || pChildNode->type() == rapidxml::node_cdata
				)
			) {
			return pChildNode->value();
		}
	}
#endif // YG_USE_TINYXML
	return NULL;
}

/*
@func		: attribute
@brief		:
*/
const char* XmlElement::attribute( const char* name, const char* pszDefValue/* = NULL*/) const {
#ifdef YG_USE_TINYXML
	if (xml_node_) {
		TiXmlElement* xmlElem = xml_node_->ToElement();
		if (xmlElem) {
			return xmlElem->Attribute(name);
		}
	}
#else
	if (IsElement(xml_node_)) {
		RdXmlAttribute* attribNode = xml_node_->first_attribute(name);
		if (attribNode){
			return attribNode->value();
		}
	}
#endif // YG_USE_TINYXML
	return pszDefValue;
}

/*
@func		: attributeInt
@brief		:
*/
int XmlElement::attributeInt(const char* name, int nDefValue/* = 0*/) const {
#ifdef YG_USE_TINYXML
	if (xml_node_) {
		TiXmlElement* xmlElem = xml_node_->ToElement();
		if (xmlElem) {
			xmlElem->Attribute(name, &nDefValue);
		}
	}
#else
	if (IsElement(xml_node_)) {
		RdXmlAttribute* attribNode = xml_node_->first_attribute(name);
		if (attribNode) {
			const char* attribValue = attribNode->value();
      if (attribValue && attribValue[0] != '\0') {
        int attrib_value = 0;
        // only return it if conversion is succeed. modified by Von.
#if 1
        attrib_value = strtoul(attribValue, NULL, 0);
        if (errno == 0)
          nDefValue = attrib_value;
#else
        if (base::StringToInt(base::StringPiece(attribValue), &attrib_value)) {
          nDefValue = attrib_value;
        }
#endif
      }
		}
	}
#endif // YG_USE_TINYXML
	//
	return nDefValue;
}

/*
@func		: setAttribute
@brief		:
*/
void XmlElement::setAttribute(const char* name, 
                                const char* pszValue,
                                size_t szName,
                                size_t szValue) {
  if (xml_node_ && IsValidString(name)) {
#ifdef YG_USE_TINYXML
    refXmlDoc->setAttribute(name, pszValue);
#else
    RdXmlDocument* doc = refXmlDoc;
    char* my_value = NULL;
    if (IsValidString(pszValue))
    {
      my_value = allocate_string(*doc, pszValue, szValue);
    } else {
      my_value = NULL;
    }
    //
    RdXmlAttribute* attrib = xml_node_->first_attribute(name, szName, false);
    if (attrib) {
      attrib->value(my_value, szValue);
    } else {
      char* my_name = allocate_string(*doc, name, szName);
      xml_node_->append_attribute(
        doc->allocate_attribute(my_name, my_value, szName, szValue));
    }
#endif
  }
}

/*
@func		: firstAttribute
@brief		: Sets an attribute of name to a given value. The attribute
will be created if it does not exist, or changed if it does.
*/
XmlAttribute XmlElement::firstAttribute(const char* str, size_t sz) const 
{
  XmlAttribute xmlAttribute;
#ifdef YG_USE_TINYXML
  #error "not implemented"
  if (xml_node_) {
    TiXmlElement* xmlElem = xml_node_->ToElement();
    if (xmlElem) {
      xmlAttribute.xml_attribute_ = xmlElem->FirstAttribute();
    }
  }
#else
  if (IsElement(xml_node_)) {
    xmlAttribute.xml_attribute_ = 
      xml_node_->first_attribute(str, sz);
  }
#endif
  //
  return xmlAttribute;
}


/*
@func		: firstChildElement
@brief		:
*/
XmlElement XmlElement::firstChildElement(const char* xmlTag/* = NULL*/) const {
	XmlElement xmlElemObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlElemObj.xml_node_ = xml_node_->FirstChildElement(xmlTag);
		} else {
			xmlElemObj.xml_node_ = xml_node_->FirstChildElement();
		}
#else
		RdXmlNode* _pXmlElem = xml_node_->first_node(xmlTag);
		if (IsElement(xml_node_)) {
			xmlElemObj.xml_node_ = _pXmlElem;
		}
#endif
	}
	return xmlElemObj;
}


/*
@func		: nextSiblingElement
@brief		: Navigate to a sibling node.
*/
XmlElement XmlElement::nextSiblingElement(const char* xmlTag/* = NULL*/) const {
	XmlElement xmlElemObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlElemObj.xml_node_ = xml_node_->NextSiblingElement(xmlTag);
		} else {
			xmlElemObj.xml_node_ = xml_node_->NextSiblingElement();
		}
#else
		RdXmlNode* _pXmlElem = xml_node_->next_sibling(xmlTag);
		if (IsElement(_pXmlElem)) {
			xmlElemObj.xml_node_ = _pXmlElem;
		}
#endif
	}
	return xmlElemObj;
}

/*
@func			: NewElement
@brief		:
*/
XmlElement XmlElement::appendElement(const char* pszTag, size_t sz) {
  XmlElement xmlElemObj;
  if (isValid()) 
  {
    xmlElemObj = document().allocateElement(pszTag, sz);
    appendElement(xmlElemObj);
  }
  return xmlElemObj;
}

XmlElement XmlElement::insertElement(const XmlElement& where, 
                                const char* pszTag, 
                                size_t sz)
{
  XmlElement xmlElemObj;
  if (isValid()) 
  {
    xmlElemObj = document().allocateElement(pszTag, sz);
    insertElement(where, xmlElemObj);
  }
  return xmlElemObj;
}

// Node manipulation
//! Prepends a new child node.
//! The prepended child becomes the first child, and all existing children are moved one position back.
//! \param child Node to prepend.
void XmlElement::prependElement(const XmlElement& node) {
  if (!IsElement(node.xml_node_)) {
    return;
  }
  prependNode(node);
}
//! Appends a new child node. 
//! The appended child becomes the last child.
//! \param child Node to append.
void XmlElement::appendElement(const XmlElement& node) {
  if (!IsElement(node.xml_node_)) {
    return;
  }
  appendNode(node);
}
//! Inserts a new child node at specified place inside the node. 
//! All children after and including the specified node are moved one position back.
//! \param where Place where to insert the child, or 0 to insert at the back.
//! \param child Node to insert.
void XmlElement::insertElement(const XmlElement& where, const XmlElement& child) {
  // where不必一定要是一个element? by Von @2015-06-12 17:42
  if (!IsElement(child.xml_node_)) {
    return;
  }
  insertNode(where, child);
}
//! Removes specified child from the node
// \param child Pointer to child to be removed.
// 删除之后，child指向的节点依然存在，只是从树中移除了。
// 返回下一个节点.
XmlElement XmlElement::removeElement(const XmlElement& child) {
  // 判断是否elment? by Von @ 2015-06-12 17:44
  XmlElement next_;
  // 自己的子节点.
  if (child.isValid() && child.xml_node_->parent() == xml_node_) {
    next_ = child.nextSiblingElement();
    xml_node_->remove_node(child.xml_node_);
  }
  return next_;
}

/*
@func			: parentElemnt
@brief		:
*/
XmlElement XmlElement::parentElemnt() const {
	XmlElement xmlElemObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
    xmlElemObj.xml_node_ = xml_node_->parent();
#else
		RdXmlNode* _pXmlElem = xml_node_->parent();
		if (_pXmlElem && rapidxml::node_element == _pXmlElem->type()) {
			xmlElemObj.xml_node_ = _pXmlElem;
		}
#endif
	}
	return xmlElemObj;
}
//////////////////////////////////////////////////////////////////////////
// HMXmlDocumentBase
XmlDocumentBase::XmlDocumentBase()
  : xml_doc_(nullptr)
  , xml_error_(false) {
}

/*
@func		: isValid
@brief		: 是否可用。
*/
bool XmlDocumentBase::isValid() const {
	return (xml_doc_ != NULL);
}

/*
@func		: isEqual
@brief		: 是否相同。
*/
bool XmlDocumentBase::isEqual(const XmlDocumentBase& other) const {
	return (xml_doc_ == other.xml_doc_);
}

/*
@func		: isError
@brief		:
*/
bool XmlDocumentBase::isError() const {
	return xml_error_;
}

/*
@func		: rootElement
@brief		:
*/
XmlElement XmlDocumentBase::rootElement(const char* xmlTag/* = NULL*/) const {
	XmlElement xmlRoot;
#ifdef YG_USE_TINYXML
	if (IsValidString(xmlTag)) {
		xmlRoot.xml_node_ = xml_doc_->FirstChildElement(xmlTag);
	} else {
		xmlRoot.xml_node_ = xml_doc_->RootElement();
	}
#else
		RdXmlNode* xml_node = xml_doc_->first_node(xmlTag);
    if (xml_node && rapidxml::node_element != xml_node->type()) { // 考虑到第一个Node不一定Element，极可能是xml声明结点。
      do 
      {
        xml_node = xml_doc_->first_node(NULL);
        if (rapidxml::node_element == xml_node->type()) {
          if (IsValidString(xmlTag)) {
            if (!strcmp(xmlTag, xml_node->name())) {
              break;
            }
          } else {
            break;
          }
        }
        // next
        xml_node = xml_node->next_sibling(NULL);
      } while (xml_node);
    }
    xmlRoot.xml_node_ = xml_node;
#endif
	//
	return xmlRoot;
}

/*
@func		: loadFromString
@brief		:
*/
bool XmlDocumentBase::loadFromString(const char* xmlString, size_t sz/* = 0*/) {
#ifdef YG_USE_TINYXML
	xml_error_ = true;
	if (IsValidString(xmlString)) {
		xml_doc_->Parse((const char*)xmlString, 0, TIXML_ENCODING_UTF8);
		xml_error_ = xml_doc_->Error();
    if (!error_msg_.empty())
    {
      error_msg_.append("\n");
    }
    error_msg_.append(xml_doc_->ErrorDesc());
	}
#else
  // YY Browser Begin
  // Modified by Von @2014-2-10 19:25:22 
  char* p = allocate_string(*xml_doc_, xmlString, sz);
  // YY Browser End
	xml_error_ = false;
	try {
		xml_doc_->parse<rapidxml::parse_trim_whitespace|rapidxml::parse_validate_closing_tags|rapidxml::parse_no_element_values|rapidxml::parse_pi_nodes>((char*)p);
	} catch (rapidxml::parse_error* e) {
    if (!error_msg_.empty())
    {
      error_msg_.append("\n");
    }
    error_msg_.append(e->what());
		xml_error_ = true;
	} catch (const std::exception& e) {
    if (!error_msg_.empty())
    {
      error_msg_.append("\n");
    }
    error_msg_.append(e.what());
		xml_error_ = true;
	}
#endif
	//
	return !xml_error_;
}

/*
@func			:  allocateElement
@brief		:
*/
XmlElement XmlDocumentBase::allocateElement(const char* pszTag, size_t sz) {
  XmlElement xmlElemObj;
#ifdef YG_USE_TINYXML
#error "not implemented"
#else
  char* my_tag = allocate_string(*xml_doc_, pszTag, sz);
  xmlElemObj.xml_node_ = 
    xml_doc_->allocate_node(rapidxml::node_element, my_tag, nullptr, sz);
#endif // !YG_USE_TINYXML
  return xmlElemObj;
}

/*
@func			: shallowCopy
@brief		: make a shallow copy of source, return the copied node.
*/
XmlNode XmlDocumentBase::shallowCopy(const XmlNode& source)
{
  XmlNode node_copy;
#ifdef YG_USE_TINYXML
#error "not implemented"
#else
  if (isValid() && source.isValid())
  {
    node_copy.xml_node_	= xml_doc_->clone_node(source.xml_node_);
  }
#endif
  return node_copy;
}

/*
@func			: print
@brief		:
*/
void XmlDocumentBase::print(std::string& out) const {
  if (xml_doc_) {
    out.assign("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    rootElement().print(out);
  }
}

//////////////////////////////////////////////////////////////////////////
// HMXmlDocument
XmlDocument::XmlDocument()
{
  _new();
}

XmlDocument::XmlDocument(const XmlDocument& src) 
{
  _new();   //不复制xml_doc。by ZC. 2015-5-20 17:29.
}

void XmlDocument::_new() {
#ifdef YG_USE_TINYXML
	// sizeof(TiXmlDocument) = 72
	char error_msg[STORAGESIZE < sizeof(TiXmlDocument) ? -1 : 1];
  (void)error_msg;
	#ifdef new
		#pragma push_macro("new")
		#undef new
		xml_doc_ = ::new (storage__) TiXmlDocument();
		#pragma pop_macro("new")
	#else
		xml_doc_ = ::new (storage__) TiXmlDocument();
	#endif // new
#else
	// sizeof(RdXmlDocument) = 65604
	char error_msg[STORAGESIZE < sizeof(RdXmlDocument) ? -1 : 1];
  (void)error_msg;
	#ifdef new
		#pragma push_macro("new")
		#undef new
		xml_doc_ = ::new (storage__) RdXmlDocument();
		#pragma pop_macro("new")
	#else
		xml_doc_ = ::new (storage__) RdXmlDocument();
	#endif // new
#endif // !YG_USE_TINYXML
}

XmlDocument::~XmlDocument() {
#ifdef YG_USE_TINYXML
	xml_doc_->~TiXmlDocument();
#else
	xml_doc_->~RdXmlDocument();
#endif // !YG_USE_TINYXML
}

}; // namespace yx
// -------------------------------------------------------------------------
