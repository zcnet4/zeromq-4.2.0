/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\socket_manager.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-25 16:28
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef SOCKET_MANAGER_H_
#define SOCKET_MANAGER_H_
#include <stdint.h>
#include <memory>
#include <unordered_map>
#include "yx_export.h"
// -------------------------------------------------------------------------
namespace yx {
class Tcp;
class Udp;
class Packet;
class PacketView;
class Loop;
class LoopImpl;
struct ITcpDelegate;
//////////////////////////////////////////////////////////////////////////
// SocketManager
class SocketManager
{
public:
  /*
  @func			: TcpListen
  @brief		:
  */
  YX_EXPORT uint64_t TcpListen(const char* host_addr, uint32_t op_type);
  /*
  @func			: TcpConnect
  @brief		:
  */
  YX_EXPORT uint64_t TcpConnect(const char* host_addr, uint32_t op_type);
  /*
  @func			: UdpListen
  @brief		:
  */
  YX_EXPORT uint64_t UdpListen(const char* host_addr, uint32_t op_type);
  /*
  @func			: UdpConnect
  @brief		:
  */
  YX_EXPORT uint64_t UdpConnect(uint64_t listen_fd, const char* host_addr, uint32_t op_type);
  /*
  @func			: Close
  @brief		:
  */
  YX_EXPORT bool Close(uint64_t fd_id);
  /*
  @func			: Send
  @brief		: 默认写[len]包长字段
  */
  YX_EXPORT bool Send(uint64_t fd_id, const yx::Packet& v1);
  /*
  @func			: Send
  @brief		: 默认写[len]包长字段
  */
  YX_EXPORT bool Send(uint64_t fd_id, const yx::PacketView& v1);
  /*
  @func			: Send2
  @brief		: 默认写[len]包长字段
  */
  YX_EXPORT bool Send2(uint64_t fd_id, const yx::PacketView& v1, const yx::PacketView& v2);
  /*
  @func			: loop
  @brief		: 
  */
  YX_EXPORT yx::Loop* loop() { return loop_; }
  /*
  @func			: set_deleagte
  @brief		:
  */
  YX_EXPORT void set_deleagte(ITcpDelegate* d);
  /*
  @func			: Shutdown
  @brief		:
  */
  YX_EXPORT void Shutdown();
protected:
  /*
  @func			: SockNew
  @brief		:
  */
  uint64_t SockNew(void* obj, bool is_tcp);
  /*
  @func			: SockFree
  @brief		:
  */
  void SockFree(uint64_t id);
  /*
  @func			: WillDestroy
  @brief		: 
  */
  void WillDestroy();
private:
  SocketManager(Loop* loop);
  ~SocketManager();
  friend class LoopImpl;
  class Sock;
  Sock* SockGet(uint64_t fd_id);
private:
  Loop*     loop_;
  typedef std::unordered_map<uint64_t/*fd_id*/, Sock*> sockets_t;
  sockets_t sockets_;
  class Dummy;
  std::unique_ptr<Dummy> dummy_;
  ITcpDelegate* tcp_deleagte_;
};

}; // namespace yx



// -------------------------------------------------------------------------
#endif /* SOCKET_MANAGER_H_ */
