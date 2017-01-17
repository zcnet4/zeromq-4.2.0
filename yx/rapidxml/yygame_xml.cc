/* -------------------------------------------------------------------------
//	FileName	：	third_party/rapidxml/yygame_xml.cc
//	Creator		：	(zc)
//	CreateTime	：	2013-04-18 10:08:19
//	Description	：	
//
// -----------------------------------------------------------------------*/
//#include "third_party/rapidxml/yygame_xml.h"
#include "yygame_xml.h"
//#include "base/compiler_specific.h"
//#include "base/strings/string_util.h"	// for base::strcasecmp
//#include "base/strings/string_number_conversions.h" // for base::StringToInt
#ifdef YG_USE_TINYXML
	#include "tinyxml.h"		// for tinyxml
  #define refXmlDoc   (xml_node_->GetDocument())
#else
	#include "rapidxml.hpp"		// for rapidxml
  #include "rapidxml_print.hpp"
	class RdXmlNode		  : public rapidxml::xml_node<char> {};
	class RdXmlAttribute: public rapidxml::xml_attribute<char> {};
	class RdXmlDocument	: public rapidxml::xml_document<char> {};
  #define refXmlDoc   (static_cast<RdXmlDocument*>(xml_node_->document()))
  
  // 判断node是否一个elment
  inline bool IsElement(RdXmlNode* node) {
    return node && node->type() == rapidxml::node_element;
  }
#endif // _DEBUG

inline int _StrCaseCmp(const char* s1, const char* s2) {
  return _stricmp(s1, s2);
}

//YYBrowser Begin
//by ZC. 2014-3-27 16:04.
#ifndef IsValidString
  #define IsValidString(x) static_cast<bool>(x && x[0])
#endif
//YYBrowser End

// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// HMXmlAttribute
HMXmlAttribute::HMXmlAttribute()
	: xml_attribute_(NULL) {
}

/*
@func		: isValid
@brief		: 是否可用。
*/
bool HMXmlAttribute::isValid() const {
	return (xml_attribute_ != NULL);
}

/*
@func		: isEqual
@brief		: 是否相同。
*/
bool HMXmlAttribute::isEqual(const HMXmlAttribute& other) const {
	return (xml_attribute_ == other.xml_attribute_);
}

/*
@func		: Name
@brief		: 
*/
const char*	HMXmlAttribute::getName() const {
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
const char*	HMXmlAttribute::getValue() const {
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
@func		: Next
@brief		: 
*/
HMXmlAttribute HMXmlAttribute::nextAttribute() const {
	HMXmlAttribute xmlAttribute;
	if (xml_attribute_ != NULL) {
#ifdef YG_USE_TINYXML
		xmlAttribute.xml_attribute_ = xml_attribute_->Next();
#else
		xmlAttribute.xml_attribute_ = static_cast<RdXmlAttribute*>(xml_attribute_->next_attribute());
#endif
	}
	return xmlAttribute;
}

//////////////////////////////////////////////////////////////////////////
// HMXmlNode
HMXmlNode::HMXmlNode()
	: xml_node_(NULL) {

}

/*
@func		: isValid
@brief		: 是否可用。
*/
bool HMXmlNode::isValid() const {
	return (xml_node_ != NULL);
}

/*
@func		: isEqual
@brief		: 是否相同。
*/
bool HMXmlNode::isEqual(const HMXmlNode& other) const {
	return (xml_node_ == other.xml_node_);
}

/*
@func		: firstChild
@brief		:
*/
HMXmlNode HMXmlNode::firstChild(const char* xmlTag/* = NULL*/) const {
	HMXmlNode xmlNodeObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlNodeObj.xml_node_ = xml_node_->FirstChild(xmlTag);
		} else {
			xmlNodeObj.xml_node_ = xml_node_->FirstChild();
		}
#else
		xmlNodeObj.xml_node_	= static_cast<RdXmlNode*>(xml_node_->first_node(xmlTag));
#endif // _DEBUG
		
	}
	return xmlNodeObj;
}

/*
@func		: lastChild
@brief		:
*/
HMXmlNode HMXmlNode::lastChild(const char* xmlTag/* = NULL*/) const {
	HMXmlNode xmlNodeObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlNodeObj.xml_node_ = xml_node_->LastChild(xmlTag);
		} else {
			xmlNodeObj.xml_node_ = xml_node_->LastChild();
		}
#else
		xmlNodeObj.xml_node_	= static_cast<RdXmlNode*>(xml_node_->last_node(xmlTag));
#endif
	}
	return xmlNodeObj;
}

/*
@func		: previousSibling
@brief		:
*/
HMXmlNode HMXmlNode::previousSibling(const char* xmlTag/* = NULL*/) const {
	HMXmlNode xmlNodeObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlNodeObj.xml_node_ = xml_node_->PreviousSibling(xmlTag);
		} else {
			xmlNodeObj.xml_node_ = xml_node_->PreviousSibling();
		}
#else
		xmlNodeObj.xml_node_	= static_cast<RdXmlNode*>(xml_node_->previous_sibling(xmlTag));
#endif
	}
	return xmlNodeObj;
}

/*
@func		: nextSibling
@brief		: Navigate to a sibling node.
*/
HMXmlNode HMXmlNode::nextSibling(const char* xmlTag/* = NULL*/) const {
	HMXmlNode xmlNodeObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlNodeObj.xml_node_ = xml_node_->NextSibling(xmlTag);
		} else {
			xmlNodeObj.xml_node_ = xml_node_->NextSibling();
		}
#else
		xmlNodeObj.xml_node_	= static_cast<RdXmlNode*>(xml_node_->next_sibling(xmlTag));
#endif
	}
	return xmlNodeObj;
}

/*
@func			: document
@brief		:
*/
HMXmlDocumentBase HMXmlNode::document() {
  if (!xml_node_) return HMXmlDocumentBase(0);

  HMXmlDocumentBase xmlDoc(refXmlDoc);
  //
  return xmlDoc;
}

/*
@func			: print
@brief		:
*/
void HMXmlNode::print(std::string& out) {
  if (xml_node_)
    rapidxml::print(std::back_inserter(out), *xml_node_, 0);
}

/*
@func			: parent
@brief		: 返回父节点
*/
HMXmlNode HMXmlNode::parent() {
  HMXmlNode xmlNodeObj;
#ifdef YG_USE_TINYXML
    xmlNodeObj.xml_node_ = xml_node_->parent();
#else
		xmlNodeObj.xml_node_	= static_cast<RdXmlNode*>(xml_node_->parent());
#endif
  return xmlNodeObj;
}

///////////////////////////////////////////////////////////////////////////
// Node manipulation
//! Prepends a new child node.
//! The prepended child becomes the first child, and all existing children are moved one position back.
//! \param child Node to prepend.
void HMXmlNode::prependNode(const HMXmlNode& node) {
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
void HMXmlNode::appendNode(const HMXmlNode& node) {
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
void HMXmlNode::insertNode(const HMXmlNode& where, const HMXmlNode& child) {
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

HMXmlNode HMXmlNode::removeNode(const HMXmlNode& child) {
  HMXmlNode next_node;
  // 的确是自己的子节点.
  if (child.isValid() && child.xml_node_->parent() == xml_node_) {
    next_node = child.nextSibling();
    xml_node_->remove_node(child.xml_node_);
  }
  return next_node;
}

// remove all child nodes of $node.
void HMXmlNode::removeAllChildren() {
  assert(isValid());
  xml_node_->remove_all_nodes();
}

void HMXmlNode::removeAllAttributes() {
  assert(isValid());
  xml_node_->remove_all_attributes();
}
//////////////////////////////////////////////////////////////////////////
// HMXmlElement
/*
@func		: tagName
@brief		: 如果当前节点是element，则返回其标签名。
*/
const char* HMXmlElement::tagName() const {
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
bool HMXmlElement::isTagName(const char* pszTag) {
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
const char* HMXmlElement::getText() const {
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
		RdXmlNode* pChildNode = static_cast<RdXmlNode*>(xml_node_->first_node());
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
const char* HMXmlElement::attribute( const char* name, const char* pszDefValue/* = NULL*/) const {
#ifdef YG_USE_TINYXML
	if (xml_node_) {
		TiXmlElement* xmlElem = xml_node_->ToElement();
		if (xmlElem) {
			return xmlElem->Attribute(name);
		}
	}
#else
	if (IsElement(xml_node_)) {
		RdXmlAttribute* attribNode = static_cast<RdXmlAttribute*>(xml_node_->first_attribute(name));
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
int HMXmlElement::attributeInt(const char* name, int nDefValue/* = 0*/) const {
#ifdef YG_USE_TINYXML
	if (xml_node_) {
		TiXmlElement* xmlElem = xml_node_->ToElement();
		if (xmlElem) {
			xmlElem->Attribute(name, &nDefValue);
		}
	}
#else
	if (IsElement(xml_node_)) {
		RdXmlAttribute* attribNode = static_cast<RdXmlAttribute*>(xml_node_->first_attribute(name));
		if (attribNode) {
			const char* attribValue = attribNode->value();
      if (attribValue && attribValue[0] != '\0') {
        int attrib_value = 0;
        // only return it if conversion is succeed. modified by Von.
#if 1
        attrib_value = atoi(attribValue);
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
void HMXmlElement::setAttribute(const char* name, const char* pszValue) {
  if (xml_node_ && IsValidString(name)) {
#ifdef YG_USE_TINYXML
    refXmlDoc->setAttribute(name, pszValue);
#else
    //#define refXmlDoc(x) (static_cast<RdXmlDocument*>(xml_node_->document<RAPIDXML_STATIC_POOL_SIZE / 2>()))
    RdXmlDocument* doc = refXmlDoc;
    const char* my_value = NULL;
    if (IsValidString(pszValue)) {
      my_value = doc->allocate_string(pszValue);
    } else {
      my_value = "";
    }
    //
    RdXmlAttribute* attrib = static_cast<RdXmlAttribute*>(xml_node_->first_attribute(name, 0, false));
    if (attrib) {
      attrib->value(my_value);
    } else {
      const char* my_name = doc->allocate_string(name);
      xml_node_->append_attribute(doc->allocate_attribute(my_name, my_value));
    }
#endif
  }
}

/*
@func		: firstAttribute
@brief		: Sets an attribute of name to a given value. The attribute
will be created if it does not exist, or changed if it does.
*/
HMXmlAttribute HMXmlElement::firstAttribute() const {
  HMXmlAttribute xmlAttribute;
#ifdef YG_USE_TINYXML
  if (xml_node_) {
    TiXmlElement* xmlElem = xml_node_->ToElement();
    if (xmlElem) {
      xmlAttribute.xml_attribute_ = xmlElem->FirstAttribute();
    }
  }
#else
  if (IsElement(xml_node_)) {
    xmlAttribute.xml_attribute_ = static_cast<RdXmlAttribute*>(xml_node_->first_attribute());
  }
#endif
  //
  return xmlAttribute;
}


/*
@func		: firstChildElement
@brief		:
*/
HMXmlElement HMXmlElement::firstChildElement(const char* xmlTag/* = NULL*/) const {
	HMXmlElement xmlElemObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlElemObj.xml_node_ = xml_node_->FirstChildElement(xmlTag);
		} else {
			xmlElemObj.xml_node_ = xml_node_->FirstChildElement();
		}
#else
		RdXmlNode* _pXmlElem = static_cast<RdXmlNode*>(xml_node_->first_node(xmlTag));
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
HMXmlElement HMXmlElement::nextSiblingElement(const char* xmlTag/* = NULL*/) const {
	HMXmlElement xmlElemObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
		if (IsValidString(xmlTag)) {
			xmlElemObj.xml_node_ = xml_node_->NextSiblingElement(xmlTag);
		} else {
			xmlElemObj.xml_node_ = xml_node_->NextSiblingElement();
		}
#else
		RdXmlNode* _pXmlElem = static_cast<RdXmlNode*>(xml_node_->next_sibling(xmlTag));
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
HMXmlElement HMXmlElement::appendElement(const char* pszTag) {
  HMXmlElement xmlElemObj;
  if (xml_node_) {
    xmlElemObj = document().allocateElement(pszTag);
    if (xmlElemObj.isValid()) {
#ifdef YG_USE_TINYXML
#else
      xml_node_->append_node(xmlElemObj.xml_node_);
#endif // !YG_USE_TINYXML
    }
  }
  return xmlElemObj;
}

// Node manipulation
//! Prepends a new child node.
//! The prepended child becomes the first child, and all existing children are moved one position back.
//! \param child Node to prepend.
void HMXmlElement::prependElement(const HMXmlElement& node) {
  if (!IsElement(node.xml_node_)) {
    return;
  }
  prependNode(node);
}
//! Appends a new child node. 
//! The appended child becomes the last child.
//! \param child Node to append.
void HMXmlElement::appendElement(const HMXmlElement& node) {
  if (!IsElement(node.xml_node_)) {
    return;
  }
  appendNode(node);
}
//! Inserts a new child node at specified place inside the node. 
//! All children after and including the specified node are moved one position back.
//! \param where Place where to insert the child, or 0 to insert at the back.
//! \param child Node to insert.
void HMXmlElement::insertElement(const HMXmlElement& where, const HMXmlElement& child) {
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
HMXmlElement HMXmlElement::removeElement(const HMXmlElement& child) {
  // 判断是否elment? by Von @ 2015-06-12 17:44
  HMXmlElement next_;
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
HMXmlElement HMXmlElement::parentElemnt() {
	HMXmlElement xmlElemObj;
	if (xml_node_) {
#ifdef YG_USE_TINYXML
    xmlElemObj.xml_node_ = xml_node_->parent();
#else
		RdXmlNode* _pXmlElem = static_cast<RdXmlNode*>(xml_node_->parent());
		if (_pXmlElem && rapidxml::node_element == _pXmlElem->type()) {
			xmlElemObj.xml_node_ = _pXmlElem;
		}
#endif
	}
	return xmlElemObj;
}
//////////////////////////////////////////////////////////////////////////
// HMXmlDocumentBase
HMXmlDocumentBase::HMXmlDocumentBase(RdXmlDocument* xmlDoc)
  : xml_doc_(xmlDoc), xml_error_(false) {

}

/*
@func		: isValid
@brief		: 是否可用。
*/
bool HMXmlDocumentBase::isValid() const {
	return (xml_doc_ != NULL);
}

/*
@func		: isEqual
@brief		: 是否相同。
*/
bool HMXmlDocumentBase::isEqual(const HMXmlDocumentBase& other) const {
	return (xml_doc_ == other.xml_doc_);
}

/*
@func		: isError
@brief		:
*/
bool HMXmlDocumentBase::isError() const {
	return xml_error_;
}

/*
@func		: rootElement
@brief		:
*/
HMXmlElement HMXmlDocumentBase::rootElement(const char* xmlTag/* = NULL*/) const {
	HMXmlElement xmlRoot;
#ifdef YG_USE_TINYXML
	if (IsValidString(xmlTag)) {
		xmlRoot.xml_node_ = xml_doc_->FirstChildElement(xmlTag);
	} else {
		xmlRoot.xml_node_ = xml_doc_->RootElement();
	}
#else
		RdXmlNode* xml_node = static_cast<RdXmlNode*>(xml_doc_->first_node(xmlTag));
    if (xml_node && rapidxml::node_element != xml_node->type()) { // 考虑到第一个Node不一定Element，极可能是xml声明结点。
      do 
      {
        xml_node = static_cast<RdXmlNode*>(xml_doc_->first_node(NULL));
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
        xml_node = static_cast<RdXmlNode*>(xml_node->next_sibling(NULL));
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
bool HMXmlDocumentBase::loadFromString(const char* xmlString, size_t sz/* = 0*/) {
#ifdef YG_USE_TINYXML
	xml_error_ = true;
	if (IsValidString(xmlString)) {
		xml_doc_->Parse((const char*)xmlString, 0, TIXML_ENCODING_UTF8);
		xml_error_ = xml_doc_->Error();
	}
#else
  // YY Browser Begin
  // Modified by Von @2014-2-10 19:25:22 
	char*p = xml_doc_->allocate_string(xmlString, sz ? sz + 1 : 0);
  if (sz != 0) {
    p[sz] = '\0';
  }
  // YY Browser End
	xml_error_ = false;
	try {
		xml_doc_->parse<rapidxml::parse_trim_whitespace|rapidxml::parse_validate_closing_tags|rapidxml::parse_no_element_values|rapidxml::parse_pi_nodes>((char*)p);
	} catch (rapidxml::parse_error* e) {
#ifdef _DEBUG
		e->what();
#else
		(void)e;
#endif // _DEBUG
		xml_error_ = true;
	} catch (...) {
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
HMXmlElement HMXmlDocumentBase::allocateElement(const char* pszTag) {
  HMXmlElement xmlElemObj;
#ifdef YG_USE_TINYXML

#else
  const char* my_tag = xml_doc_->allocate_string(pszTag);
  xmlElemObj.xml_node_ = static_cast<RdXmlNode*>(xml_doc_->allocate_node(rapidxml::node_element, my_tag));
#endif // !YG_USE_TINYXML
  return xmlElemObj;
}

/*
@func			: print
@brief		:
*/
void HMXmlDocumentBase::print(std::string& out) const {
  if (xml_doc_) {
    out.assign("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    rootElement().print(out);
  }
}

//////////////////////////////////////////////////////////////////////////
// HMXmlDocument
HMXmlDocument::HMXmlDocument()
  : HMXmlDocumentBase(NULL) {
  _new();
}

HMXmlDocument::HMXmlDocument(const HMXmlDocument& src) 
  : HMXmlDocumentBase(NULL) {
  _new();   //不复制xml_doc。by ZC. 2015-5-20 17:29.
}

void HMXmlDocument::_new() {
#ifdef YG_USE_TINYXML
	// sizeof(TiXmlDocument) = 72
	char error_msg[STORAGESIZE < sizeof(TiXmlDocument) ? -1 : 1];
	error_msg[0];
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
	error_msg[0];
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

HMXmlDocument::~HMXmlDocument() {
#ifdef YG_USE_TINYXML
	xml_doc_->~TiXmlDocument();
#else
	xml_doc_->~RdXmlDocument();
#endif // !YG_USE_TINYXML
}

// -------------------------------------------------------------------------
