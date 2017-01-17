/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\netproto.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-5 11:04
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef NETPROTO_H_
#define NETPROTO_H_
#include <stdint.h>
// -------------------------------------------------------------------------
//[len][cmd][session][uid][content]:包长度+命令+会话+uid+内容。
//sizeof(uint16_t),sizeof(uint16_t),sizeof(uint32_t),sizeof(uint32_t)
//////////////////////////////////////////////////////////////////////////
// NETCMD_C2S
enum class NETCMD_C2S : uint16_t {
  MASK = 0x0FFF,
  CONNECT = 0x0001,
  CHALLENGE_RESPONSE = 0x0002,
  JOIN_SERVER = 0x0003,
  REQ_CONN_GATEWAY = 0x0004,
};

//////////////////////////////////////////////////////////////////////////
// NETCMD_S2C
enum class NETCMD_S2C : uint16_t {
  MASK = 0x0FFF,
  CONNECT = 0x1001,
  CHALLENGE = 0x1002,
  CONNECT_ACCEPT = 0x1003,
  CONNECT_REJECT = 0x1004,
  JOIN_SERVER_RESPONSE = 0x1005,
  REQ_CONN_GATEWAY_RESPONSE = 0x1006,
};

//////////////////////////////////////////////////////////////////////////
// NETCMD_G2G: gateway-forward <--> gateway
enum class NETCMD_G2G : uint16_t {
  MASK = 0x0FFF,
  GF2G_FORWARD = 0x2001,
  GF2G_FORWARD_CLOSE = 0x2002,
  G2GF_FORWARD = 0x2003,
};
// -------------------------------------------------------------------------
#endif /* NETPROTO_H_ */
