/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\yx_constants.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-27 11:09
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef YX_CONSTANTS_H_
#define YX_CONSTANTS_H_
#include "yx_constants_macros.h"
// -------------------------------------------------------------------------
#define FW_DEFINE_CONST(const_name, const_value) _CHROME_DEFINE_CONST_(k, const_name, const_value)

namespace yx {

const int kPackageLen = 64 * 1024; // 64K
//FW_DEFINE_CONST(PackageLen, 16 * 1024);

}; // namespace yx
// -------------------------------------------------------------------------
#endif /* FW_CONSTANTS_H_ */
