/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\gateway_config.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-1 15:00
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "gateway_config.h"
#include "gateway_constants.h"
#include "gateway_util.h"
#include "build/compiler_specific.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// Config,网关配置类。
Config::Config()
{
  reader_.Open(kConfigFileName);
  current_dir_ = reader_.GetItemString(nullptr, "current_dir", ".");
  if ("." == current_dir_) {
    yx::util::PathSplit(kConfigFileName, &current_dir_, nullptr, nullptr);
  }
}

Config::~Config()
{

}

const char* Config::kConfigFileName = kGATECFG_FileName;

Config& Config::Instance() {
  static Config inst;
  return inst;
}

/*
@func			: getCurrentDir
@brief		:
*/
const char* Config::getCurrentDir() const {
  return current_dir_.c_str();
}

/*
@func			: GetRealPath
@brief		:
*/
std::string Config::GetRealPath(const char* path) {
#ifdef OS_WIN
  if (*path == '\\') {
    return path;
  }
#else
  if (*path == '/') {
    return path;
  }
#endif // OS_WIN
  return current_dir_ + path;
}

/*
@func			:  getGatewayName
@brief		:
*/
const char* Config::getGatewayName() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_Name, "defalut");
}

/*
@func			: getMachineId
@brief		:
*/
uint32_t Config::getMachineId() const {
  return reader_.GetItemInt(kGATECFG_Gate, kGATECFG_MID, 0);
}

/*
@func			:  getGatewayPWD
@brief		:
*/
const char* Config::getGatewayPWD() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_PWD, "defalut");
}

/*
@func			:  getMasterHost
@brief		:  获取Master主机地址，Gate主动连接Master(由skynet实现)。
*/
const char* Config::getMasterHost() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_Master, "127.0.0.1:9080");
}

/*
@func			:  getFrontendHost
@brief		:
*/
const char* Config::getFrontendHost() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_Frontend, nullptr);
}

/*
@func			: getBackendHost
@brief		:
*/
const char* Config::getBackendHost() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_Backend, nullptr);
}

/*
@func			: getProxyTcpHost
@brief		:
*/
const char* Config::getProxyTcpHost() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_ProxyTcp, nullptr);
}

/*
@func			: getProxyUdpHost
@brief		:
*/
const char* Config::getProxyUdpHost() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_ProxyUdp, nullptr);
}

/*
@func			: getPingPongHost
@brief		:
*/
const char* Config::getPingPongHost() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_PingPong, nullptr);
}
/*
@func			: getPBClient
@brief		:
*/
const char* Config::getPBClient() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_PBClient, "proto_client.pb");
}

/*
@func			: getPBServer
@brief		:
*/
const char* Config::getPBServer() const {
  return reader_.GetItemString(kGATECFG_Gate, kGATECFG_PBServer, "proto_server.pb");
}

/*
@func			: getZmqServerNum
@brief		:
*/
uint32_t Config::getZmqServerNum() const {
  return reader_.GetItemInt(nullptr, kGATECFG_ZmqServerNum, 0);
}


/*
@func			: getZmqServerAndWorldId
@brief		:
*/
const char* Config::getZmqServerAndWorldId(uint32_t index, uint32_t& world_id) const {
  char world_id_key[64] = { 0 };
  sprintf(world_id_key, "zmq_server_%u_world_id", index);
  world_id = reader_.GetItemInt(nullptr, world_id_key, 0);
  //
  char zmq_server_key[64] = { 0 };
  sprintf(zmq_server_key, "zmq_server_%u", index);
  return reader_.GetItemString(nullptr, zmq_server_key, nullptr);
}

/*
@func			: getSpawnWorldIds
@brief		:
*/
std::vector<std::string> Config::getSpawnWorldIds() {
  const char* spawn_world_id_string = reader_.GetItemString(nullptr, kGATECFG_SpawnWorldIds, nullptr);
  if (spawn_world_id_string && *spawn_world_id_string) {
    return gateway_util::split(spawn_world_id_string, ',');
  }
  //
  return std::vector<std::string>();
}

// -------------------------------------------------------------------------
