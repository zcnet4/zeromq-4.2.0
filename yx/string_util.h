/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\string_util.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-15 17:09
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_
#include "yx_export.h"
// -------------------------------------------------------------------------
namespace yx {
  /*
  @func			: strcasecmp
  @brief		: 
  */
  YX_EXPORT int strcasecmp(const char* s1, const char* s2);
  /*
  @func			: strcasecmp16
  @brief		:
  */
  YX_EXPORT int strcasecmp16(const wchar_t* s1, const wchar_t* s2);
}; // namespace yx

// -------------------------------------------------------------------------
#endif /* STRING_UTIL_H_ */
