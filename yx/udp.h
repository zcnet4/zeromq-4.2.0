/* -------------------------------------------------------------------------
//    FileName		:	D:\yx_code\yx\yx\udp.h
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-12-24 13:57
//    Description	:    
//
// -----------------------------------------------------------------------*/
#ifndef UDP_H_
#define UDP_H_
#include "yx_export.h"
#include <memory>
#include "tcp_delegate.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
struct IKCPCB;
union sockaddr_all;
namespace yx {
class Loop;
//////////////////////////////////////////////////////////////////////////
// Udp，设计上与yx::Tcp概念上保持一致，表示一个“连接”。by ZC. 2016-12-24 22:56
class YX_EXPORT Udp {
public:
  enum class EStatus : uint8_t {
    disconnect = 0,
    listening,
    connecting,
    connected,
    action,
  };
  static Udp* New(Loop* loop);
public:
  /*
  @func			: Bind
  @brief		:
  */
  int Bind(const char* host_addr);
  /*
  @func			: Listen
  @brief		:
  */
  int Listen(const char* host_addr);
  /*
  @func			: ReadStart
  @brief		:
  */
  int ReadStart();
  /*
  @func			: ReadStop
  @brief		:
  */
  int ReadStop();
  /*
  @func			: Connect
  @brief		:
  */
  int Connect(const char* host_addr, uint64_t& connect_fd);
  /*
  @func			: Send
  @brief		: 
  */
  int Send(const uint8_t* buf, uint16_t buf_size);
  /*
  @func			: Close
  @brief		:
  */
  void Close();
  /*
  @func			: set_delegate
  @brief		:
  */
  void set_delegate(IUdpDelegate* d);

  uint64_t fd_id() const { return fd_id_; }
protected:
  static int on_kcp_output_cb(const char *buf, int len, IKCPCB *kcp, void *user);
private:
  class Core;
  friend class Core;
  Udp(Core* core);
  Udp(const Udp&) = delete;
  Udp& operator=(const Udp&) = delete;
  ~Udp();
  //
  void* operator new(size_t);
  void* operator new(size_t, void* ptr){ return ptr; }
  void operator delete(void*);
  sockaddr_all* addr() {
    return reinterpret_cast<sockaddr_all*>(this + 1);
  }
  static Udp* Spawn(Core* core, const sockaddr_all* addr);
private:
  EStatus                 status_;
  std::shared_ptr<Core>   core_;
  bool                    spawn_;
  IKCPCB*                 kcp_;
  uint64_t                fd_id_;
};


}; // namespace yx

// -------------------------------------------------------------------------
#endif /* UDP_H_ */