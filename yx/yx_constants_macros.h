/* -------------------------------------------------------------------------
//	FileName	£∫	base/yx_constants_macros.h
//	Creator		£∫	(zc) <zcnet4@gmail.com>
//	CreateTime	£∫	2013-10-24 14:58:31
//	Description	£∫	
//
// -----------------------------------------------------------------------*/

// -------------------------------------------------------------------------
#ifdef _USE_DEFINE_CONST_
	#undef _YGD_DEFINE_CONST_
	#define _YGD_DEFINE_CONST_(name_prefix, const_name, const_value) char name_prefix##_##const_name[] = #const_value
	//
	#undef _CHROME_DEFINE_CONST_
	#define _CHROME_DEFINE_CONST_(name_prefix, const_name, const_value) char name_prefix##const_name[] = #const_value
#else
	#undef _YGD_DEFINE_CONST_
	#define _YGD_DEFINE_CONST_(name_prefix, const_name, const_value) extern char name_prefix##_##const_name[]
	//
	#undef _CHROME_DEFINE_CONST_
	#define _CHROME_DEFINE_CONST_(name_prefix, const_name, const_value) extern char name_prefix##const_name[]
#endif // _USE_DEFINE_CONST_

//////////////////////////////////////////////////////////////////////////
// ”√∑®£∫
/*
const_def.h:
#include "base/yx_constants_macros.h"
_YGD_DEFINE_CONST_(name, value);

const_def.cc:
//#undef CONST_DEF_H_
#define _USE_DEFINE_CONST_
#include "const_def.h"
*/
// -------------------------------------------------------------------------
