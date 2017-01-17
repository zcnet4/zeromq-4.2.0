/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\yx_util.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-14 11:19
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef YX_UTIL_H_
#define YX_UTIL_H_
#include <stdint.h>
#include <vector>
#include <string>
#include "yx_export.h"
// -------------------------------------------------------------------------
namespace yx {
namespace util {
/*
@func			: GetCurrentDirectory
@brief		: 
*/
bool YX_EXPORT GetCurrentDirectory(std::string* dir);
/*
@func			: PathSplit
@brief		: 
*/
void YX_EXPORT PathSplit(const char* path, std::string* dir, std::string* name, std::string* ext);
/*
@func			: ReadFileToBuffer
@brief		: 
*/
bool YX_EXPORT ReadFileToBuffer(const char* file, std::vector<uint8_t>* buf);

/*
@func			: TrimString
@brief		:
*/
void YX_EXPORT TrimString(std::string& str, const char* trim = " \t\n\r\v");
/*
@func			: multwait_new
@brief		: 
*/
YX_EXPORT void* multwait_new(unsigned int count);
/*
@func			: multwait_wait
@brief		: ¶à¸öµÈ´ý
*/
bool YX_EXPORT multwait_wait(void* mw);

}; // namespace util
}; // namespace yx


// -------------------------------------------------------------------------
#endif /* YX_UTIL_H_ */
