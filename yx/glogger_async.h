/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\glogger_async.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-24 15:32
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef GLOGGER_ASYNC_H_
#define GLOGGER_ASYNC_H_
#include "yx_export.h"
// -------------------------------------------------------------------------
void YX_EXPORT async_glogger(int level);
YX_EXPORT void*  async_glogger_get(int level);
void YX_EXPORT async_glogger_set(int level, void* l);
// -------------------------------------------------------------------------
#endif /* GLOGGER_ASYNC_H_ */
