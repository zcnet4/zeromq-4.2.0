/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\socket_manager.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-25 16:28
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "socket_manager.h"
#include "logging.h"
#include "loop.h"
#include "loop_impl.h"
#include "tcp.h"
#include "udp.h"
#include "allocator.h"
#include <vector>
#include "tcp_delegate.h"
#include "packet_view.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// SocketManager::Sock
class SocketManager::Sock
  : public YXAllocObject<>
{
public:
  static const uint32_t kConnectTimeOut = 1 * 60 * 1000; // 1分钟，连接超时
  static const uint32_t kRecvTimeOut = 5 * 60 * 1000; // 5分钟，接收超时
public:
  static Sock* New(yx::LoopImpl* loop_impl) {
    return new Sock(loop_impl);
  }
  void Destroy() {
    uv_timer_stop(&timer_);
    uv_close((uv_handle_t*)&timer_, Sock::close_cb);
  }
  void Check(uint32_t to = kConnectTimeOut) {
    timeout_ = to;
    uv_timer_start(&timer_, &Sock::timer_cb, 0, 0);
  }
  bool istcp() {
    return 0 != (flags_ & 0x80);
  }
  void set_tcp(Tcp* tcp) {
    flags_ |= 0x80;
    fd_.tcp = tcp;
  }
  Tcp* tcp() {
    return fd_.tcp;
  }
  void set_udp(Udp* udp) {
    flags_ &= 0x7F;
    fd_.udp = udp;
  }
  Udp* udp() {
    return fd_.udp;
  }
  void Close() {
    if (istcp()) {
      fd_.tcp->Close();
    } else {
      fd_.udp->Close();
    }
  }
protected:
  static void close_cb(uv_handle_t* handle) {
    Sock* sock = reinterpret_cast<Sock*>(handle->data);
    delete sock;
  }
  static void timer_cb(uv_timer_t* handle) {
    Sock* sock = reinterpret_cast<Sock*>(handle->data);
    //
    LoopImpl* loop_impl = reinterpret_cast<LoopImpl*>(sock->timer_.loop->data);
    uint64_t ticks_now = uv_now(loop_impl->uv_loop());
    //
    sock->on_timer(loop_impl, ticks_now);
  }
  void on_timer(LoopImpl* loop_impl, uint64_t ticks_now) {
    if (flags_ & 0x1) {
      if (ticks_now > fd_.tcp->recv_time() + timeout_) {
        fd_.tcp->Close();
        return;
      }
      Check(kRecvTimeOut);
    } else {
      // 心跳
      if (ticks_now > fd_.tcp->send_time() + timeout_) {
        Packet p(0);
        //fd_.tcp->Send(p);
        LOG(INFO) << "heartbeat fd:" << fd_.tcp->tcp_id();
      }
      Check(kRecvTimeOut - 60 * 1000);
    }
  }
private:
  Sock(yx::LoopImpl* loop_impl)
    : flags_(0)
  {
    uv_timer_init(loop_impl->uv_loop(), &timer_);
    timer_.data = this;
  }
  Sock(const Sock& src) = delete;
  Sock& operator=(const Sock& src) = delete;
  ~Sock() {}
private:
  uint8_t flags_;
  union {
    Tcp* tcp;
    Udp* udp;
  } fd_;
  uv_timer_t timer_;
  uint32_t   timeout_;
};
//////////////////////////////////////////////////////////////////////////
// SocketManager::Dummy
class SocketManager::Dummy
  : public yx::ITcpDelegate
{
protected:
  virtual void OnTcpConnect(yx::Tcp* connect_tcp, int err) {
    
  }
  virtual void OnTcpAccpet(yx::Tcp* tcp, yx::Tcp* accpet_tcp, int err) {
  
  }
  virtual void OnTcpRead(yx::Tcp* tcp, yx::Packet& packet) {

  }
  virtual void OnTcpClose(yx::Tcp* tcp) {

  }
};
//////////////////////////////////////////////////////////////////////////
// SocketManager
SocketManager::SocketManager(Loop* loop)
  : loop_(loop)
  , dummy_(new Dummy)
  , tcp_deleagte_(nullptr)
{
  tcp_deleagte_ = dummy_.get();
}

SocketManager::~SocketManager()
{

} 

/*
@func			: set_deleagte
@brief		:
*/
void SocketManager::set_deleagte(ITcpDelegate* d) {
  if (nullptr == d) {
    if (tcp_deleagte_ != dummy_.get()) {
      tcp_deleagte_ = dummy_.get();
      for (auto iter(sockets_.begin()), iterEnd(sockets_.end()); iterEnd != iter; ++iter) {
        Sock*& sock = iter->second;
        sock->tcp()->set_delegate(tcp_deleagte_);
      }
    }
  } else {
    tcp_deleagte_ = d;
  }
}

/*
@func			: TcpListen
@brief		:
*/
uint64_t SocketManager::TcpListen(const char* host_addr, uint32_t op_type) {
  yx::Tcp* tcp = yx::Tcp::New(loop_);
  tcp->set_delegate(tcp_deleagte_);
  tcp->set_tcp_type(op_type & ~kTCP_MASK);
  int err = tcp->Listen(host_addr);
  if (0 == err) {
    return tcp->tcp_id();
  }
  //
  Close(tcp->tcp_id());
  return 0;
}

/*
@func			: TcpConnect
@brief		:
*/
uint64_t SocketManager::TcpConnect(const char* host_addr, uint32_t op_type) {
  yx::Tcp* tcp = yx::Tcp::New(loop_);
  tcp->set_delegate(tcp_deleagte_);
  tcp->set_tcp_type(op_type & ~kTCP_MASK);
  tcp->set_nodelay(true);
  int err = tcp->Connect(host_addr);
  if (0 == err) {
    return tcp->tcp_id();
  }
  //
  LOG(WARNING) << "SocketManager::Connect :" << host_addr << " err [" << uv_err_name(err) << "]:" << uv_strerror(err);
  Close(tcp->tcp_id());
  return 0;
}

/*
@func			: UdpListen
@brief		:
*/
uint64_t SocketManager::UdpListen(const char* host_addr, uint32_t op_type) {
  yx::Udp* udp = yx::Udp::New(loop_);
  int err = udp->Listen(host_addr);
  if (0 == err) {
    return udp->fd_id();
  }
  return 0;
}

/*
@func			: UdpConnect
@brief		:
*/
uint64_t SocketManager::UdpConnect(uint64_t listen_fd, const char* host_addr, uint32_t op_type) {
  Sock* sock = SockGet(listen_fd);
  if (!sock || sock->istcp()) {
    return 0;
  }
  Udp* listen_udp = sock->udp();
  uint64_t connect_fd = 0;
  int err = listen_udp->Connect(host_addr, connect_fd);
  if (0 == err) {
    return connect_fd;
  }
  //
  return 0;
}

/*
@func			: Close
@brief		:
*/
bool SocketManager::Close(uint64_t fd_id) {
  Sock* sock = SockGet(fd_id);
  if (sock) {
    sock->Close();
    return true;
  }
  return false;
}

/*
@func			: Send
@brief		:
*/
bool SocketManager::Send(uint64_t tcp_id, const yx::Packet& v1) {
  Sock* sock = SockGet(tcp_id);
  if (!sock) {
    LOG(WARNING) << "SocketManager::Send Not exist fd_id:" << tcp_id;
    return false;
  }
  if (sock->istcp()) {
    sock->tcp()->Send(yx::PacketView(v1));
  } else {
    sock->udp()->Send(v1.buf(), v1.buf_size());
  }
  //
  return false;
}

/*
@func			: Send
@brief		:
*/
bool SocketManager::Send(uint64_t tcp_id, const yx::PacketView& v1) {
  Sock* sock = SockGet(tcp_id);
  if (!sock) {
    LOG(WARNING) << "SocketManager::Send Not exist fd_id:" << tcp_id;
    return false;
  }
  if (sock->istcp()) {
    sock->tcp()->Send(v1);
  } else {
    sock->udp()->Send(v1.buf(), v1.buf_size());
  }
  //
  return false;
}

/*
@func			: Send2
@brief		:
*/
bool SocketManager::Send2(uint64_t tcp_id, const yx::PacketView& v1, const yx::PacketView& v2) {
  Sock* sock = SockGet(tcp_id);
  if (!sock) {
    LOG(WARNING) << "SocketManager::Send Not exist fd_id:" << tcp_id;
    return false;
  }
  if (sock->istcp()) {
    sock->tcp()->Send2(v1, v2);
  } else {
    sock->udp()->Send(v1.buf(), v1.buf_size());
    sock->udp()->Send(v2.buf(), v2.buf_size());
  }
  //
  return false;
}

/*
@func			: Shutdown
@brief		:
*/
void SocketManager::Shutdown() {
  // 退出时，将deleagte恢复为dummy。by ZC. 2017-1-11 11:44.
  tcp_deleagte_ = dummy_.get();
  //
  std::vector<Sock*> socks;
  socks.reserve(sockets_.size());
  for (auto iter(sockets_.begin()), iterEnd(sockets_.end()); iterEnd != iter; ++iter) {
    Sock*& sock = iter->second;
    sock->tcp()->set_delegate(tcp_deleagte_);
    socks.push_back(sock);
  }
  //
  for (size_t i = 0, count = socks.size(); i < count; ++i) {
    socks[i]->Close();
  }
}

/*
@func			: WillDestroy
@brief		:
*/
void SocketManager::WillDestroy() {
  DCHECK(sockets_.size() == 0);
}

/*
@func			: SockNew
@brief		:
*/
uint64_t SocketManager::SockNew(void* obj, bool is_tcp) {
  uint64_t fd_id = 0;
  //
  Sock* sock = Sock::New(loop_->loop_impl());
  if (is_tcp) {
    sock->set_tcp(reinterpret_cast<Tcp*>(obj));
  } else {
    sock->set_udp(reinterpret_cast<Udp*>(obj));
  }
  uint64_t loop_count = 0;
  do {
    fd_id = loop_->loop_impl()->alloc_fd();
    std::pair<sockets_t::iterator, bool> result = sockets_.emplace(sockets_t::value_type(fd_id, sock));
    if (result.second) {
      break;
    }
    if (++loop_count > 1000){
      loop_->loop_impl()->time_refresh();
      loop_count = 0;
    }
  } while (true);
  //
  return fd_id;
}

/*
@func			: SockFree
@brief		:
*/
void SocketManager::SockFree(uint64_t id) {
  auto iter(sockets_.find(id));
  if (sockets_.end() == iter) return;
  //
  Sock* sock = iter->second;
  sockets_.erase(iter);
  sock->Destroy();
}

SocketManager::Sock* SocketManager::SockGet(uint64_t fd_id) {
  auto iter = sockets_.find(fd_id);
  if (sockets_.end() != iter) {
    return iter->second;
  }
  //
  return nullptr;
}

}; // namespace yx
// -------------------------------------------------------------------------
