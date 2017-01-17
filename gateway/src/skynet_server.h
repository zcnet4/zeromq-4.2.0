/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\skynet_server.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-29 11:28
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef SKYNET_SERVER_H_
#define SKYNET_SERVER_H_
#include <stdint.h>
#include <map>
#include <memory>
#include "yx/processor.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// 
namespace yx {
  class Timer;
  class Packet;
  class SocketManager;
}
//////////////////////////////////////////////////////////////////////////
// SkynetServer，处理连接后端Skynet网络，用于网关发现，控制，监控等功能。
class SkynetServer
{
public:
  SkynetServer();
  ~SkynetServer();
public:
  /*
  @func			: Start
  @brief		: 
  */
  void Start(yx::SocketManager* socket_manager);
  /*
  @func			: Stop
  @brief		: 
  */
  void Stop();
public:
  /*
  @func			: HandleConnect
  @brief		:
  */
  void HandleConnect(uint64_t fd, yx::Packet& packet);
  /*
  @func			: HandleClose
  @brief		:
  */
  void HandleClose(uint64_t fd, yx::Packet& packet);
  /*
  @func			: HandleData
  @brief		:
  */
  bool HandleData(uint64_t tcp_id, yx::Packet& packet);
private:
  /*
  @func			: ConnectAP
  @brief		: 连接Skynet AP服务。
  */
  uint64_t ConnectAP(const char ap_host[64], const char ap_server[64]);
  /*
  @func			: ReqLoginGateway
  @brief		: 
  */
  bool ReqConnGateway(uint64_t ap_id);
  /*
  @func			: OnReconnect
  @brief		: 
  */
  void OnReconnect(const std::string& ap_host, const std::string& ap_server, yx::Timer* timer);
private:
  bool stopping_;
  yx::SocketManager* socket_manager_;
  struct AP;
  typedef std::map<uint64_t/*tcp_id/ap_id*/, std::unique_ptr<AP>> ap_map_t;
  ap_map_t ap_map_;
};


// -------------------------------------------------------------------------
#endif /* SKYNET_SERVER_H_ */
