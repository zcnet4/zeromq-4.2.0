/* -------------------------------------------------------------------------
//	FileName	��	third_party/rapidxml/yygame_xml.h
//	Creator		��	(zc)
//	CreateTime	��	2013-04-18 10:08:26
//	Description	��	
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
  typedef TiXmlNode       RdXmlNode;          //��ࡢ͵����by ZC. 2015-2-5 17:27.
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
	@brief		: �Ƿ���á�
	*/
	bool isValid() const;
	/*
	@func		: isEqual
	@brief		: �Ƿ���ͬ��
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
	@brief		: �Ƿ���á�
	*/
	bool isValid() const;
	/*
	@func		: isEqual
	@brief		: �Ƿ���ͬ��
	*/
	bool isEqual(const HMXmlNode& other) const;
	/*
	@func		: unspecified_bool_type
	@brief		: ����boolֵ�ж���by ZC. 2012-10-19 10:49.
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
  @brief		: ���ظ��ڵ�
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
  // ɾ��֮��childָ��Ľڵ���Ȼ���ڣ�ֻ�Ǵ������Ƴ��ˡ�
  // ������һ���ڵ�.
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
	@brief		: �����ǰ�ڵ���element���򷵻����ǩ����
	*/
	const char* tagName() const;
	/*
	@func		: isTagName
	@brief		: �Ƿ���ָ���ı�ǩ����
	*/
	bool isTagName(const char* pszTag);
	/*
	@func		: getText
	@brief		: �����ǰ�ڵ㺬��TiXmlText�ڵ㣬�򷵻���Ӧ��ֵ�����򣬷���NULL
				  ���ַ��汾�򷵻ء����ַ���
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
  // ɾ��֮��childָ��Ľڵ���Ȼ���ڣ�ֻ�Ǵ������Ƴ��ˡ�
  // ������һ���ڵ�.
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
	@brief		: �Ƿ���á�
	*/
	bool isValid() const;
	/*
	@func		: isEqual
	@brief		: �Ƿ���ͬ��
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
	@brief		: ���|xmlString|������'\0'��β��sz��Ҫ����'\0'������ʵ��ʱ����ϡ�
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
