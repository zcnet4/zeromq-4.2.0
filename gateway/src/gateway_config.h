/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\gateway_config.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-1 15:00
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef GATEWAY_CONFIG_H_
#define GATEWAY_CONFIG_H_
#include <stdint.h>
#include <vector>
#include <string>
#include "yx/ini_reader.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// Config,网关配置类。
class Config 
{
public:
  static Config& Instance();
  static const char* kConfigFileName;
public:
  /*
  @func			: getCurrentDir
  @brief		: 
  */
  const char* getCurrentDir() const;
  /*
  @func			: GetRealPath
  @brief		: 
  */
  std::string GetRealPath(const char* path);
  /*
  @func			:  getGatewayName
  @brief		:
  */
  const char* getGatewayName() const;
  /*
  @func			: getMachineId
  @brief		:
  */
  uint32_t getMachineId() const;
  /*
  @func			:  getGatewayPWD
  @brief		:
  */
  const char* getGatewayPWD() const;
  /*
  @func			:  getMasterHost
  @brief		:  获取Master主机地址，Gateway主动连接Master(由skynet实现)。
  */
  const char* getMasterHost() const;
  /*
  @func			:  getFrontendHost
  @brief		: 
  */
  const char* getFrontendHost() const;
  /*
  @func			: getBackendHost
  @brief		: 
  */
  const char* getBackendHost() const;
  /*
  @func			: getProxyTcpHost
  @brief		:
  */
  const char* getProxyTcpHost() const;
  /*
  @func			: getProxyUdpHost
  @brief		:
  */
  const char* getProxyUdpHost() const;
  /*
  @func			: getPingPongHost
  @brief		:
  */
  const char* getPingPongHost() const;
  /*
  @func			: getPBClient
  @brief		:
  */
  const char* getPBClient() const;
  /*
  @func			: getPBServer
  @brief		:
  */
  const char* getPBServer() const;
  /*
  @func			: getZmqServerNum
  @brief		: 
  */
  uint32_t getZmqServerNum() const;
  /*
  @func			: getZmqServerAndWorldId
  @brief		: 
  */
  const char* getZmqServerAndWorldId(uint32_t index, uint32_t& world_id) const;
  /*
  @func			: getSpawnWorldIds
  @brief		: 
  */
  std::vector<std::string> getSpawnWorldIds();
private:
  Config();
  ~Config();
  std::string       current_dir_;
  yx::IniFileReader reader_;
};


// -------------------------------------------------------------------------
#endif /* GATEWAY_CONFIG_H_ */
