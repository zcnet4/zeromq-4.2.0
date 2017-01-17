/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\yx_export.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-1 14:21
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef YX_EXPORT_H_
#define YX_EXPORT_H_

// -------------------------------------------------------------------------

#if defined(COMPONENT_BUILD) && !defined(COMPILE_YX_STATICALLY)
#if defined(WIN32)

#if defined(YX_IMPLEMENTATION)
#define YX_EXPORT __declspec(dllexport)
#else
#define YX_EXPORT __declspec(dllimport)
#endif  // defined(YX_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(YX_IMPLEMENTATION)
#define YX_EXPORT __attribute__((visibility("default")))
#else
#define YX_EXPORT
#endif
#endif

#else // defined(COMPONENT_BUILD)
#define YX_EXPORT
#endif

// -------------------------------------------------------------------------
#endif /* YX_EXPORT_H_ */
