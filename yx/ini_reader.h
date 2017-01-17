/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\ini_reader.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-28 17:04
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef INI_READER_H_
#define INI_READER_H_
#include <map>
#include "yx_export.h"
#include "yx_stl_util.h"
// -------------------------------------------------------------------------
namespace yx {

//////////////////////////////////////////////////////////////////////////
// IniReader
class YX_EXPORT IniReader {
public:
  IniReader();
  ~IniReader();
public:
  /*
  @func			: Parse
  @brief		: 
  */
  bool Parse(char* buf, int buf_size);
  /*
  @func			: Clear
  @brief		: 
  */
  void Clear();
  /*
  @func			: GetItemString
  @brief		: 
  */
  const char* GetItemString(const char* session, const char* key, const char* def_value) const;
  /*
  @func			: GetItemInt
  @brief		:
  */
  int GetItemInt(const char* session, const char* key, int def_value) const;
  /*
  @func			: EnumKey
  @brief		: 
  */
  bool EnumKey(const char* session, int& index, std::string& key) const;
private:
  // Ini格式中键值表。
  typedef std::map<std::string, std::string, less_i<std::string> > keyvalues_t;
  // Ini格式中段表示。
  typedef std::map<std::string, keyvalues_t*, less_i<std::string> > sessions_t;
  sessions_t sessions_;
};

//////////////////////////////////////////////////////////////////////////
// IniFileReader
class YX_EXPORT IniFileReader
  : public IniReader
{
public:
  /*
  @func			: Open
  @brief		: 
  */
  bool Open(const char* fileName);
};

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* INI_READER_H_ */
