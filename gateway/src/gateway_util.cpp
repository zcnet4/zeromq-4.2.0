/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\gateway_util.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 14:39
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "gateway_util.h"
#include <string>
// -------------------------------------------------------------------------
namespace gateway_util {

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> tokens;
  std::string current = "";
  bool last_was_escape = false;

  if (s.size() == 0)
    return tokens;
  for (unsigned int i = 0; i < s.size(); i++) {
    if (last_was_escape) {
      current += '\\';
      current += s.c_str()[i];
      last_was_escape = false;
    } else {
      if (s.c_str()[i] == delim) {
        tokens.push_back(current);
        current = "";
        last_was_escape = false;
      } else if (s.c_str()[i] == '\\') {
        last_was_escape = true;
      } else {
        current += s.c_str()[i];
        last_was_escape = false;
      }
    }
  }
  //push last element
  tokens.push_back(current);
  return tokens;
}
}; // namespace gateway_util
// -------------------------------------------------------------------------
