/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\gateway_constants.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-1 15:15
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef GATEWAY_CONSTANTS_H_
#define GATEWAY_CONSTANTS_H_
#include "yx/yx_constants_macros.h"
// -------------------------------------------------------------------------
#define GATE_DEFINE_CONST(const_name, const_value) _YGD_DEFINE_CONST_(kGATE, const_name, const_value)
// ≈‰÷√
#define CONFIG_DEFINE_CONST(const_name, const_value) _YGD_DEFINE_CONST_(kGATECFG, const_name, const_value)
#ifndef YX_GF
CONFIG_DEFINE_CONST(FileName, gateway.conf);
#else
CONFIG_DEFINE_CONST(FileName, gf.conf);
#endif // YX_GF
CONFIG_DEFINE_CONST(MID, machine_id);
CONFIG_DEFINE_CONST(Gate, gate);
CONFIG_DEFINE_CONST(Name, name);
CONFIG_DEFINE_CONST(PWD, pwd);
CONFIG_DEFINE_CONST(Master, master_host);
CONFIG_DEFINE_CONST(Frontend, frontend_host);
CONFIG_DEFINE_CONST(ProxyTcp, proxy_tcp);
CONFIG_DEFINE_CONST(ProxyUdp, proxy_udp);
CONFIG_DEFINE_CONST(Backend, backend_host);
CONFIG_DEFINE_CONST(PingPong, pingpong_host);
CONFIG_DEFINE_CONST(PBClient, pb_client);
CONFIG_DEFINE_CONST(PBServer, pb_server);
CONFIG_DEFINE_CONST(ZmqServerNum, zmq_server_num);
CONFIG_DEFINE_CONST(SpawnWorldIds, spawn_world_ids);
//
#define PROTO_DEFINE_CONST(const_name, const_value) _YGD_DEFINE_CONST_(kPROTO, const_name, const_value)
PROTO_DEFINE_CONST(Message, Message);
PROTO_DEFINE_CONST(MessageType, type);
PROTO_DEFINE_CONST(MessageMsgData, msg_data);
PROTO_DEFINE_CONST(QueuedMsg, QueuedMsg);
PROTO_DEFINE_CONST(QueuedMsgUid, uid);
PROTO_DEFINE_CONST(QueuedMsgType, type);
PROTO_DEFINE_CONST(QueuedMsgData, data);
PROTO_DEFINE_CONST(C2SLogin, c2s_login);
PROTO_DEFINE_CONST(C2SLoginUid, uid);
PROTO_DEFINE_CONST(C2SLoginWorldId, world_id);
PROTO_DEFINE_CONST(C2SLogout, c2s_logout);
PROTO_DEFINE_CONST(C2SLogoutParam, param);
PROTO_DEFINE_CONST(S2SOnlineNum, s2s_online_num);
PROTO_DEFINE_CONST(S2SOnlineNumWorldId, world_id);
PROTO_DEFINE_CONST(S2SOnlineNumOnlineNum, online_num);
PROTO_DEFINE_CONST(S2CAccessDenied, s2c_access_denied);
PROTO_DEFINE_CONST(S2CAccessDeniedCode, code);
PROTO_DEFINE_CONST(S2CAccessDeniedInfo, info);
PROTO_DEFINE_CONST(S2CSwitchWorld, s2c_switch_world);
PROTO_DEFINE_CONST(S2CSwitchWorldResult, result);
PROTO_DEFINE_CONST(S2CSwitchWorldToWorldId, to_world_id);


// -------------------------------------------------------------------------
#endif /* GATEWAY_CONSTANTS_H_ */
