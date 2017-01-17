/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\ini_reader.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-28 17:04
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "ini_reader.h"
#include <fstream>
#include "yx_util.h"
#include <string.h>
// -------------------------------------------------------------------------

namespace yx {

//////////////////////////////////////////////////////////////////////////
//
IniReader::IniReader()
{

}


IniReader::~IniReader()
{
  Clear();
}

/*
@func			: Parse
@brief		:
*/
bool IniReader::Parse(char* buf, int buf_size) {
  static char delim[] = "\r\n";		// Ini 文件换行符 Tokens
  char* token = NULL;
  //char* pNextToken = NULL;

  keyvalues_t* kv = new keyvalues_t;
  // 用注释“;”来表示全局的节。by ZC. 2010-4-28
  sessions_.insert(sessions_t::value_type("", kv));
  //
  std::string line;
  for (token = strtok(buf, delim); token != NULL; token = strtok(NULL, delim)) {
    // 先查找注释符号“；”。
    char* comment = strchr(token, ';');
    if (comment)
      line.assign(token, comment - token);
    else
      line.assign(token);

    yx::util::TrimString(line);
    if (line.size() > 3) {
      // 为了简单只认定第一位和最后一位是“[]”才是一段开始。
      if ('[' == (*line.begin()) && ']' == (*line.rbegin())) {
        line.erase(line.size() - 1, 1);
        line.erase(0, 1);
        //
        kv = new keyvalues_t;
        sessions_.insert(sessions_t::value_type(line, kv));
      } else if (kv) {
        std::string::size_type uPos = line.find('=');
        if (std::string::npos != uPos) {
          std::string key, value;

          line.substr(0, uPos).swap(key);
          line.substr(uPos + 1, line.size() - uPos - 1).swap(value);

          //key.Trim(), value.Trim();
          yx::util::TrimString(key);
          yx::util::TrimString(value);
          kv->insert(keyvalues_t::value_type(key, value));
        }
      }
    } // if line.size() > 3
  } // for 
  //
  return true;
}

/*
@func			: Clear
@brief		:
*/
void IniReader::Clear() {
  sessions_t sessions_del;
  sessions_del.swap(sessions_);
  //
  for (sessions_t::iterator iter(sessions_del.begin()), iterEnd(sessions_del.end()); iterEnd != iter; ++iter) {
    delete iter->second;
  }
}

/*
@func			: GetItemString
@brief		:
*/
const char* IniReader::GetItemString(const char* session, const char* key, const char* def_value) const {
  sessions_t::const_iterator iter = sessions_.find(session && *session ? std::string(session) : "");
  if (sessions_.end() != iter) {
    keyvalues_t* kv = iter->second;
    if (kv) {
      keyvalues_t::const_iterator i = kv->find(key);
      if (kv->end() != i) {
        return i->second.c_str();
      }
    } // if kv
  }
  //
  return def_value;
}
/*
@func			: GetItemInt
@brief		:
*/
int IniReader::GetItemInt(const char* session, const char* key, int def_value) const {
  const char* val = GetItemString(session, key, NULL);
  if (NULL != val) {
    return strtoul(val, NULL, 0);
  }
  //
  return def_value;
}

/*
@func			: EnumKey
@brief		:
*/
bool IniReader::EnumKey(const char* session, int& index, std::string& key) const {
  sessions_t::const_iterator it = sessions_.find(session);
  if (sessions_.end() == it) return false;
  //
  keyvalues_t* kv = it->second;
  if (NULL == kv) return false;
  int i = 0;
  for (keyvalues_t::iterator iter(kv->begin()), iterEnd(kv->end()); iterEnd != iter; ) {
     if (i++ < index) {
       iter++;
     } else {
       key = iter->first;
       index = i;
       return true;
     }
  } // for
  return false;
}

//////////////////////////////////////////////////////////////////////////
// IniFileReader
bool IniFileReader::Open(const char* fileName) {
  bool success = false;
  std::fstream	f;
  f.open(fileName, std::ios_base::in | std::ios_base::binary);
  if (f.is_open()) {
    f.seekg(0, std::ios::end);
    int file_size = static_cast<int>(f.tellg());
    f.seekg(0);
    //
    char* buf = new char[file_size + 1];
    buf[file_size] = '\0';
    f.read(buf, std::streamsize(file_size));
    success = Parse(buf, file_size + 1);
    //
    delete buf;
    f.close();
  }
  //
  return success;
}

}; // namespace yx

// -------------------------------------------------------------------------
