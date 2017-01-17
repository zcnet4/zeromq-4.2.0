/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\gateway_util.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 14:39
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef GATEWAY_UTIL_H_
#define GATEWAY_UTIL_H_
#include <stdint.h>
#include <string>
#include <vector>
#include "yx/yx_util.h"
#include "yx/packet_view.h"
// -------------------------------------------------------------------------
namespace gateway_util {
/*
@func			: split
@brief		: 
*/                                                                 
std::vector<std::string> split(const std::string &s, char delim);

}; //namespace GATEWAY_UTIL_H_
// -------------------------------------------------------------------------
#endif /* UTIL_H_ */
