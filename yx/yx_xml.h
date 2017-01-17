/* -------------------------------------------------------------------------
//	FileName	：	third_party/rapidxml/yx_xml.h
//	Creator		：	(zc) <zcnet4@gmail.com>
//	CreateTime	：	2013-04-18 10:08:26
//	Description	：	
//
// -----------------------------------------------------------------------*/
#ifndef THIRD_PARTY_RAPIDXML_YX_XML_H_
#define THIRD_PARTY_RAPIDXML_YX_XML_H_

// -------------------------------------------------------------------------
//#include "base/basictypes.h"
#include "yx_export.h"
#include <string>
//////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------
#ifdef YG_USE_TINYXML
	class TiXmlNode;
	class TiXmlAttribute;
	class TiXmlDocument;
  typedef TiXmlNode       RdXmlNode;          //简洁、偷懒。by ZC. 2015-2-5 17:27.
  typedef TiXmlAttribute  RdXmlAttribute;
  typedef TiXmlDocument   RdXmlDocument;
  typedef TiXmlNode::NodeType RdXmlNodeType;
#else
  namespace rapidxml
  {
      template <typename Ch> class xml_node;
      template <typename Ch> class xml_attribute;
      template <typename Ch> class xml_document;
  };

  typedef rapidxml::xml_document<char> RdXmlDocument;
  typedef rapidxml::xml_node<char> RdXmlNode;
  typedef rapidxml::xml_attribute<char> RdXmlAttribute;
#endif // !YG_USE_TINYXML

#ifndef NULL
  #define NULL 0
#endif

namespace yx {
class XmlDocumentBase;
//////////////////////////////////////////////////////////////////////////
// XmlAttribute.
class YX_EXPORT XmlAttribute {
public:
	XmlAttribute();
	XmlAttribute(XmlAttribute&&);
  XmlAttribute& operator=(XmlAttribute&&);
	XmlAttribute(const XmlAttribute&) = default;
  XmlAttribute& operator=(const XmlAttribute&) = default;

public:
	/*
	@func		: isValid
	@brief		: 是否可用。
	*/
	bool isValid() const;
	/*
	@func		: isEqual
	@brief		: 是否相同。
	*/
	bool isEqual(const XmlAttribute& other) const;
	/*
	@func		: Name
	@brief		: 
	*/
	const char* getName() const;
	/*
	@func		: Value
	@brief		: 
	*/
	const char*	getValue() const;
	/*
	@func		: Name
	@brief		: 
	*/
  void setName(const char* str, size_t sz = 0) const;
	/*
	@func		: Value
	@brief		: 
	*/
	void setValue(const char* str, size_t sz = 0) const;
	/*
	@func		: Next
	@brief		: 
	*/
	XmlAttribute nextAttribute() const;
private:
  RdXmlAttribute*			xml_attribute_;
	friend class XmlElement;
};

//////////////////////////////////////////////////////////////////////////
// XmlNode
class YX_EXPORT XmlNode {
public:
  enum class Type
  {
    Document,      //!< A document node. Name and value are empty.
    Element,       //!< An element node. Name contains element name. Value contains text of first data node.
    Data,          //!< A data node. Name is empty. Value contains data text.
    CData,         //!< A CDATA node. Name is empty. Value contains data text.
    Comment,       //!< A comment node. Name is empty. Value contains comment text.
    Declaration,   //!< A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
    DocType,       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
    PI             //!< A PI node. Name contains target. Value contains instructions.
  };

public:
	XmlNode();
	XmlNode(XmlNode&&);
  XmlNode& operator=(XmlNode&&);
	XmlNode(const XmlNode&) = default;
  XmlNode& operator=(const XmlNode&) = default;

public:
  const char* getName() const;
  void setName(const char* name, size_t sz = 0);
  const char* getValue() const;
  void setValue(const char* name, size_t sz = 0);
	/*
	@func		: isValid
	@brief		: 是否可用。
	*/
	bool isValid() const;
	/*
	@func		: isEqual
	@brief		: 是否相同。
	*/
	bool isEqual(const XmlNode& other) const;
	/*
	@func		: isEqual
	@brief		: 是否相同。
	*/
  bool isType(Type test_type) const;
	/*
	@func		: unspecified_bool_type
	@brief		: 增加bool值判定。by ZC. 2012-10-19 10:49.
	*/
	typedef RdXmlNode* XmlNode::*unspecified_bool_type;
	operator unspecified_bool_type() const {
		return xml_node_ ? &XmlNode::xml_node_ : NULL;
	}
	/*
	@func		: firstChild
	@brief		:
	*/
	XmlNode firstChild(const char* xmlTag = NULL) const;
	/*
	@func		: lastChild
	@brief		:
	*/
	XmlNode lastChild(const char* xmlTag = NULL) const;
	/*
	@func		: previousSibling
	@brief		:
	*/
	XmlNode previousSibling(const char* xmlTag = NULL) const;
	/*
	@func		: nextSibling
	@brief		: Navigate to a sibling node.
	*/
	XmlNode nextSibling(const char* xmlTag = NULL) const;
  /*
  @func			: document
  @brief		: 
  */
  XmlDocumentBase document();
  /*
  @func			: print
  @brief		: 
  */
  void print(std::string& out);

  /*
  @func			: parent
  @brief		: 返回父节点
  */
  XmlNode parent();
  /*
  @func			: shallowCopy
  @brief		: 把当前的节点树做一份拷贝，但属性值，name,value都跟当前节点共用
              一份存储。
  */
  XmlNode shallowCopy();
  ///////////////////////////////////////////////////////////////////////////
  // Node manipulation
  //! Prepends a new child node.
  //! The prepended child becomes the first child, and all existing children are moved one position back.
  //! \param child Node to prepend.
  void prependNode(const XmlNode& node);
  //! Appends a new child node. 
  //! The appended child becomes the last child.
  //! \param child Node to append.
  void appendNode(const XmlNode& node);
  //! Inserts a new child node at specified place inside the node. 
  //! All children after and including the specified node are moved one position back.
  //! \param where Place where to insert the child, or 0 to insert at the back.
  //! \param child Node to insert.
  void insertNode(const XmlNode& where, const XmlNode& child);
  //! Removes specified child from the node
  // \param child Pointer to child to be removed.
  // 删除之后，child指向的节点依然存在，只是从树中移除了。
  // 返回下一个节点.
  XmlNode removeNode(const XmlNode& child);

  // remove all child nodes of current node.
  void removeAllChildren();

  // remove all attributes of current node.
  void removeAllAttributes();

protected:
  RdXmlNode*				xml_node_;
	friend class XmlDocumentBase;
};

//////////////////////////////////////////////////////////////////////////
// XmlElement
class YX_EXPORT XmlElement : public XmlNode {
public:
  // inherit parent constructors
  using XmlNode::XmlNode;

public:
	/*
	@func		: tagName
	@brief		: 如果当前节点是element，则返回其标签名。
	*/
	const char* tagName() const;
	/*
	@func		: isTagName
	@brief		: 是否是指定的标签名。
	*/
	bool isTagName(const char* pszTag);
	/*
	@func		: getText
	@brief		: 如果当前节点含有TiXmlText节点，则返回相应的值，否则，返回NULL
				  宽字符版本则返回“”字符串
	*/
	const char* getText() const;
	/*
	@func		: attribute
	@brief		:
	*/
	const char* attribute(const char* name, const char* pszDefValue = NULL) const;
	/*
	@func		: attributeInt
	@brief		:
	*/
	int attributeInt(const char* name, int nDefValue = 0) const;
  /*
  @func		: setAttribute
  @brief		:
  */
  void setAttribute(const char* name, 
                    const char* pszValue,
                    size_t szName = 0,
                    size_t szValue = 0);
  /*
  @func		: firstAttribute
  @brief		: Sets an attribute of name to a given value. The attribute
  will be created if it does not exist, or changed if it does.
  */
  XmlAttribute firstAttribute(const char* str = nullptr, 
                                size_t sz = 0) const;
  /*
	@func		: firstChildElement
	@brief		:
	*/
	XmlElement firstChildElement(const char* xmlTag = NULL) const;
	/*
	@func		: nextSiblingElement
	@brief		: Navigate to a sibling node.
	*/
	XmlElement nextSiblingElement(const char* xmlTag = NULL) const;
  /*
  @func			: parentElemnt
  @brief		:
  */
  XmlElement parentElemnt() const;
  /*
  @func			: appendElement
  @brief		:
  */
  XmlElement appendElement(const char* pszTag, size_t sz = 0);
  /*
  @func			: insertElement
  @brief		:
  */
  XmlElement insertElement(const XmlElement& where, 
                     const char* pszTag, size_t sz = 0);

  // Node manipulation
  //! Prepends a new child node.
  //! The prepended child becomes the first child, and all existing children are moved one position back.
  //! \param child Node to prepend.
  void prependElement(const XmlElement& node);
  //! Appends a new child node. 
  //! The appended child becomes the last child.
  //! \param child Node to append.
  void appendElement(const XmlElement& node);
  //! Inserts a new child node at specified place inside the node. 
  //! All children after and including the specified node are moved one position back.
  //! \param where Place where to insert the child, or 0 to insert at the back.
  //! \param child Node to insert.
  void insertElement(const XmlElement& where, const XmlElement& child);
  //! Removes specified child from the node
  // \param child Pointer to child to be removed.
  // 删除之后，child指向的节点依然存在，只是从树中移除了。
  // 返回下一个节点.
  XmlElement removeElement(const XmlElement& child);

};

//////////////////////////////////////////////////////////////////////////
// XmlDocumentBase
class YX_EXPORT XmlDocumentBase {
public:
  XmlDocumentBase();

public:
	/*
	@func		: isValid
	@brief		: 是否可用。
	*/
	bool isValid() const;
	/*
	@func		: isEqual
	@brief		: 是否相同。
	*/
  bool isEqual(const XmlDocumentBase& other) const;
	/*
	@func		: isError
	@brief		:
	*/
	bool isError() const;
	/*
	@func		: isError
	@brief		:
	*/
  const std::string& errorMsg() { return error_msg_; }
	/*
	@func		: rootElement
	@brief		:
	*/
	XmlElement rootElement(const char* xmlTag = NULL) const;
	/*
	@func		: loadFromString
	@brief		: 如果|xmlString|不是以'\0'结尾，sz不要加上'\0'，函数实现时会加上。
	*/
	bool loadFromString(const char* xmlString, size_t sz = 0);
  /*
  @func			:  allocateElement
  @brief		: 
  */
  XmlElement allocateElement(const char* pszTag, size_t sz = 0);
  /*
  @func			: shallowCopy
  @brief		: make a shallow copy of source, return the copied node.
  */
  XmlNode shallowCopy(const XmlNode& source);
  /*
  @func			: print
  @brief		:
  */
  void print(std::string& out) const;

protected:
	RdXmlDocument* xml_doc_;
  std::string error_msg_;
	bool		   xml_error_;
  friend class XmlNode;
};

//////////////////////////////////////////////////////////////////////////
// XmlDocument
class YX_EXPORT XmlDocument : public XmlDocumentBase {
public:
	XmlDocument();
  XmlDocument(const XmlDocument& src);
	~XmlDocument();
public:
private:
  void _new();
#ifdef YG_USE_TINYXML
  static const int STORAGESIZE = 72 + 24;
#else
  static const int STORAGESIZE = 65604 + 124;
#endif // !YG_USE_TINYXML
  char		     storage__[STORAGESIZE];
};

}; // namespace yx
// -------------------------------------------------------------------------
#endif /* THIRD_PARTY_RAPIDXML_YX_XML_H_ */
