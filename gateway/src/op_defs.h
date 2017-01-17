/* -------------------------------------------------------------------------
//    FileName		:	D:\yx_code\yx\gateway\op_defs.h
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-11-20 15:31
//    Description	:    
//
// -----------------------------------------------------------------------*/
#ifndef OP_DEFS_H_
#define OP_DEFS_H_
#include <stdint.h>
// -------------------------------------------------------------------------
// gcc太坑了，不支持enum class与switch case连用。by ZC. 2016-12-17 10:31
namespace TCP_OP {
  static const uint16_t OP_MASK = 0x0FFF;
  static const uint16_t UNKNOWN = 0x0000;
  static const uint16_t YW_SERVER = 0x0010;
  static const uint16_t YW_C2F = 0x0011;
  static const uint16_t YW_F2M = 0x0012;
  static const uint16_t YW_M2B = 0x0013;
  static const uint16_t YW_S2B = 0x0014;
  static const uint16_t YW_B2M = 0x0015;
  static const uint16_t YW_M2F = 0x0016;
  static const uint16_t YW_GF2G = 0x0017;  //转发程序到前端。
  static const uint16_t YW_G2GF = 0x0018;
  //
  static const uint16_t YX_SERVER = 0x0020;
  static const uint16_t YX_C2F = 0x0021;
  static const uint16_t YX_F2M = 0x0022;
  static const uint16_t YX_M2B = 0x0023;
  static const uint16_t YX_S2B = 0x0024;
  static const uint16_t YX_B2M = 0x0025;
  static const uint16_t YX_M2F = 0x0026;
  // 转发。
  static const uint16_t FORWARD_SERVER = 0x0030; // 转发服务。
  static const uint16_t FORWARD_C2F = 0x0031;  
  static const uint16_t FORWARD_F2B = 0x0032;
  static const uint16_t FORWARD_F2B_CLOSE = 0x0033;
  static const uint16_t FORWARD_B2F = 0x0034;  
  //
  static const uint16_t SKYNET_SERVER = 0x0040;
  static const uint16_t SKYNET_F2F = 0x0041;
  //
  static const uint16_t PINGPONG_SERVER = 0x0050;
}
// -------------------------------------------------------------------------
#endif /* OP_DEFS_H_ */