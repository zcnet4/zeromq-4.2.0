/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\forward_server.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-28 11:24
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef FORWARD_SERVER_H_
#define FORWARD_SERVER_H_
#include <map>
#include <list>
#include <vector>
#include "yx/packet.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// 
namespace yx {
  class SocketManager;
}
//////////////////////////////////////////////////////////////////////////
// ForwardServer，转发服务。
class ForwardServer
{
public:
  ForwardServer();
  ~ForwardServer();
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
  /*
  @func			: ConnectGateway
  @brief		: gateway-forward连接gateway建议信道。
  */
  void ConnectGateway(const char* gateway_host);
  /*
  @func			: Forward
  @brief		: 
  */
  void Forward(uint64_t client_id, uint32_t client_session, const yx::Packet& packet);
  /*
  @func			: ForwardClose
  @brief		:
  */
  void ForwardClose(uint64_t client_id, const yx::Packet& packet);
  /*
  @func			: HandleConnect
  @brief		: 
  */
  void HandleConnect(uint64_t tcp_id, yx::Packet& packet);
  /*
  @func			: HandleClose
  @brief		:
  */
  void HandleClose(uint64_t tcp_id, yx::Packet& packet);
  /*
  @func			: HandleData
  @brief		:
  */
  void HandleData(uint64_t tcp_id, yx::Packet& packet);
private:
  struct Channel;
  /*
  @func			: GetTcpChannel
  @brief		: 
  */
  Channel* GetTcpChannel(uint64_t client_id);
  /*
  @func			: Forward
  @brief		:
  */
  void ForwardImpl(uint64_t client_id, const yx::Packet& packet);
private:
  yx::SocketManager* socket_manager_;
  typedef std::list<uint64_t> ids_t;
  struct Channel {
    uint16_t connected;
    uint16_t mss;
    uint32_t session;       //会话
    uint64_t tcp_id;
    ids_t ids;
  };
  typedef std::vector<Channel> channels_t;
  channels_t channels_;
  typedef std::map<uint64_t/*client_id*/, Channel*> channel_cache_t;
  channel_cache_t cache_;
};


// -------------------------------------------------------------------------
#endif /* FORWARD_SERVER_H_ */
