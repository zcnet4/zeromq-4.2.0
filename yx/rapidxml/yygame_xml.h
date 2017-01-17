/* -------------------------------------------------------------------------
//	FileName	：	third_party/rapidxml/yygame_xml.h
//	Creator		：	(zc)
//	CreateTime	：	2013-04-18 10:08:26
//	Description	：	
//
// -----------------------------------------------------------------------*/
#ifndef THIRD_PARTY_RAPIDXML_YYGAME_XML_H_
#define THIRD_PARTY_RAPIDXML_YYGAME_XML_H_

// -------------------------------------------------------------------------
//#include "base/basictypes.h"
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
#else
	class RdXmlNode;
	class RdXmlAttribute;
	class RdXmlDocument;
#endif // !YG_USE_TINYXML
class HMXmlDocumentBase;

#ifndef NULL
  #define NULL 0
#endif

//////////////////////////////////////////////////////////////////////////
// HMXmlAttribute.
class HMXmlAttribute {
public:
	HMXmlAttribute();
	/*
	@func		: isValid
	@brief		: 是否可用。
	*/
	bool isValid() const;
	/*
	@func		: isEqual
	@brief		: 是否相同。
	*/
	bool isEqual(const HMXmlAttribute& other) const;
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
	@func		: Next
	@brief		: 
	*/
	HMXmlAttribute nextAttribute() const;
private:
  RdXmlAttribute*			xml_attribute_;
	friend class HMXmlElement;
};

//////////////////////////////////////////////////////////////////////////
// HMXmlNode
class HMXmlNode {
public:
	HMXmlNode();
	/*
	@func		: isValid
	@brief		: 是否可用。
	*/
	bool isValid() const;
	/*
	@func		: isEqual
	@brief		: 是否相同。
	*/
	bool isEqual(const HMXmlNode& other) const;
	/*
	@func		: unspecified_bool_type
	@brief		: 增加bool值判定。by ZC. 2012-10-19 10:49.
	*/
	typedef RdXmlNode* HMXmlNode::*unspecified_bool_type;
	operator unspecified_bool_type() const {
		return xml_node_ ? &HMXmlNode::xml_node_ : NULL;
	}
	/*
	@func		: firstChild
	@brief		:
	*/
	HMXmlNode firstChild(const char* xmlTag = NULL) const;
	/*
	@func		: lastChild
	@brief		:
	*/
	HMXmlNode lastChild(const char* xmlTag = NULL) const;
	/*
	@func		: previousSibling
	@brief		:
	*/
	HMXmlNode previousSibling(const char* xmlTag = NULL) const;
	/*
	@func		: nextSibling
	@brief		: Navigate to a sibling node.
	*/
	HMXmlNode nextSibling(const char* xmlTag = NULL) const;
  /*
  @func			: document
  @brief		: 
  */
  HMXmlDocumentBase document();
  /*
  @func			: print
  @brief		: 
  */
  void print(std::string& out);

  /*
  @func			: parent
  @brief		: 返回父节点
  */
  HMXmlNode parent();

  ///////////////////////////////////////////////////////////////////////////
  // Node manipulation
  //! Prepends a new child node.
  //! The prepended child becomes the first child, and all existing children are moved one position back.
  //! \param child Node to prepend.
  void prependNode(const HMXmlNode& node);
  //! Appends a new child node. 
  //! The appended child becomes the last child.
  //! \param child Node to append.
  void appendNode(const HMXmlNode& node);
  //! Inserts a new child node at specified place inside the node. 
  //! All children after and including the specified node are moved one position back.
  //! \param where Place where to insert the child, or 0 to insert at the back.
  //! \param child Node to insert.
  void insertNode(const HMXmlNode& where, const HMXmlNode& child);
  //! Removes specified child from the node
  // \param child Pointer to child to be removed.
  // 删除之后，child指向的节点依然存在，只是从树中移除了。
  // 返回下一个节点.
  HMXmlNode removeNode(const HMXmlNode& child);

  // remove all child nodes of current node.
  void removeAllChildren();

  // remove all attributes of current node.
  void removeAllAttributes();

protected:
  RdXmlNode*				xml_node_;
	friend class HMXmlDocumentBase;
};

//////////////////////////////////////////////////////////////////////////
// HMXmlElement
class HMXmlElement : public HMXmlNode {
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
  void setAttribute(const char* name, const char* pszValue);
  /*
  @func		: firstAttribute
  @brief		: Sets an attribute of name to a given value. The attribute
  will be created if it does not exist, or changed if it does.
  */
  HMXmlAttribute firstAttribute() const;
  /*
	@func		: firstChildElement
	@brief		:
	*/
	HMXmlElement firstChildElement(const char* xmlTag = NULL) const;
	/*
	@func		: nextSiblingElement
	@brief		: Navigate to a sibling node.
	*/
	HMXmlElement nextSiblingElement(const char* xmlTag = NULL) const;
  /*
  @func			: parentElemnt
  @brief		:
  */
  HMXmlElement parentElemnt();
  /*
  @func			: appendElement
  @brief		:
  */
  HMXmlElement appendElement(const char* pszTag);

  // Node manipulation
  //! Prepends a new child node.
  //! The prepended child becomes the first child, and all existing children are moved one position back.
  //! \param child Node to prepend.
  void prependElement(const HMXmlElement& node);
  //! Appends a new child node. 
  //! The appended child becomes the last child.
  //! \param child Node to append.
  void appendElement(const HMXmlElement& node);
  //! Inserts a new child node at specified place inside the node. 
  //! All children after and including the specified node are moved one position back.
  //! \param where Place where to insert the child, or 0 to insert at the back.
  //! \param child Node to insert.
  void insertElement(const HMXmlElement& where, const HMXmlElement& child);
  //! Removes specified child from the node
  // \param child Pointer to child to be removed.
  // 删除之后，child指向的节点依然存在，只是从树中移除了。
  // 返回下一个节点.
  HMXmlElement removeElement(const HMXmlElement& child);

};

//////////////////////////////////////////////////////////////////////////
// HMXmlDocumentBase
class HMXmlDocumentBase {
public:
  HMXmlDocumentBase(RdXmlDocument* xmlDoc);
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
  bool isEqual(const HMXmlDocumentBase& other) const;
	/*
	@func		: isError
	@brief		:
	*/
	bool isError() const;
	/*
	@func		: rootElement
	@brief		:
	*/
	HMXmlElement rootElement(const char* xmlTag = NULL) const;
	/*
	@func		: loadFromString
	@brief		: 如果|xmlString|不是以'\0'结尾，sz不要加上'\0'，函数实现时会加上。
	*/
	bool loadFromString(const char* xmlString, size_t sz = 0);
  /*
  @func			:  allocateElement
  @brief		: 
  */
  HMXmlElement allocateElement(const char* pszTag);
  /*
  @func			: print
  @brief		:
  */
  void print(std::string& out) const;
protected:
	RdXmlDocument* xml_doc_;
	bool		   xml_error_;
};

//////////////////////////////////////////////////////////////////////////
// HMXmlDocument
class HMXmlDocument : public HMXmlDocumentBase {
public:
	HMXmlDocument();
  HMXmlDocument(const HMXmlDocument& src);
	~HMXmlDocument();
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

// -------------------------------------------------------------------------

#endif /* THIRD_PARTY_RAPIDXML_YYGAME_XML_H_ */
