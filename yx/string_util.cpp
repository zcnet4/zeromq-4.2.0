/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\string_util.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-15 17:10
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "string_util.h"
#include "build/compiler_specific.h"
#include <string.h>
// -------------------------------------------------------------------------
namespace yx {
/*
@func			: strcasecmp
@brief		:
*/
int strcasecmp(const char* s1, const char* s2) {
#ifdef OS_WIN
  return stricmp(s1, s2);
#else
  return ::strcasecmp(s1, s2);
#endif // OS_WIN
}

/*
@func			: wstrcasecmp
@brief		:
*/
int strcasecmp16(const wchar_t* s1, const wchar_t* s2) {
#ifdef OS_WIN
  return wcsicmp(s1, s2);
#else
  return false;
#endif // OS_WIN
}

}; // namespace yx


// -------------------------------------------------------------------------
