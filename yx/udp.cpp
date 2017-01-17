/* -------------------------------------------------------------------------
//    FileName		:	D:\yx_code\yx\yx\udp.cpp
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-12-24 14:05
//    Description	:    
//
// -----------------------------------------------------------------------*/
#include "udp.h"
#include "uv.h"
#include "loop.h"
#include "loop_impl.h"
#include "logging.h"
#include <map>
#include "packet.h"
#include "packet_view.h"
#include "crypt.h"
#include "kcp/ikcp.h"
#include "allocator.h"
#include "_sockaddr.h"
#include "crypt.h"
extern "C" {
#include "fnv-5.0.3/fnv.h"
}
// -------------------------------------------------------------------------

namespace yx {
/*
@func			: udp_init
@brief		: 
*/
void udp_init() {
  ikcp_allocator(yx_malloc, yx_free);
}
static const uint8_t kXorKey[4] = { 0x20, 0x15, 0x10, 0x03 };
//////////////////////////////////////////////////////////////////////////
// Udp::Core
class Udp::Core
{
public:
  Core(yx::Loop* loop)
    : loop_(loop)
    , delegate_(nullptr)
    , timer_due_(0xFFFFFFFFFFFFFFFF)
  {
    CHECK(loop);
    uv_udp_init(loop_->uv_loop(), &udp_);
    udp_.data = this;
    uv_timer_init(loop_->uv_loop(), &timer_);
    timer_.data = this;
  }
  ~Core()
  {
    uv_timer_stop(&timer_);
  }
  static const uint32_t kListeningConv = 0;
  static const uint16_t kMTU = 1500;  // 以太网的MTU值是1500bytes
  static const uint64_t kKCPClock = 40;
  static const uint32_t kKCPFast = 20;
public:
  int Bind(const sockaddr_all* addr) {
    return uv_udp_bind(&udp_, &addr->s, 0);
  }
  
  int ReadStart() {
    int err = uv_udp_recv_start(&udp_, &Core::on_recv_alloc_cb, &Core::on_recv_read_cb);
    //
    return err;
  }
  
  int ReadStop() {
    return uv_udp_recv_stop(&udp_);
  }
  
  int SendTo(const sockaddr_all* addr, const uint8_t* ptr, uint16_t buf_size) {
    if (buf_size > Core::kMTU) return UV_EMSGSIZE;
    uv_buf_t buf[2];
    // 可用ringbuffer优化。
    buf[1] = uv_buf_init((char*)yx_malloc(buf_size), buf_size);
    memcpy(buf[1].base, ptr, buf_size);
    yx::crypt::xor_string((uint8_t*)buf[1].base, buf_size, kXorKey, 4);
    //
    // 优化write req分配。
    uv_udp_send_t* req = reinterpret_cast<uv_udp_send_t*>(loop_->loop_impl()->alloc_uv_req());
    req->data = buf[1].base;
    //
    buf[0].base = reinterpret_cast<char*>(req + 1);
    buf[0].len = sizeof(uint32_t);
    _write_u32((uint8_t*)buf[0].base, fnv_32a_buf(buf[1].base, buf_size, FNV1_32A_INIT));
    //
    //send_time_ = loop_->ticks_now();
    return uv_udp_send(req, &udp_, buf, 2, &addr->s, &Core::on_udp_send_cb);
  }
  uint32_t alloc_convid() const {
    IUINT32 conv_key[2] = { 0 };
    yx::crypt::randomkey(reinterpret_cast<uint8_t*>(conv_key));
    return conv_key[0];
  }
  bool register_udp(uint32_t convid, Udp* udp) {
    auto r = conns_.insert(conns_t::value_type(convid, udp));
    return r.second;
  }
  Loop* loop() {
    return loop_;
  }
  void set_delegate(IUdpDelegate* d) {
    delegate_ = d;
  }
  void TimerStart(uint32_t timeout) {
    uint64_t due = loop_->ticks_now() + timeout;
    if (due < timer_due_) {
      uv_timer_start(&timer_, Core::on_timer_cb, timeout, 0);
      timer_due_ = due;
    }
  }
  void TimerStop() {
    uv_timer_stop(&timer_);
  }
protected:
  static void on_udp_send_cb(uv_udp_send_t* req, int status) {
    yx_free(req->data);
    LoopImpl::free_uv_req(req);
    // 如果写失败，则关闭Close。
    if (status != 0) {
      LOG(ERROR) << "Udp write error:" << status;
      //Udp* udp = reinterpret_cast<Udp*>(req->handle->data);
      //LOG(ERROR) << "Udp write failure is turned off. tcp_id:" << tcp->tcp_id();
      //tcp->Close();
    }
  }
  static void on_recv_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    Udp::Core* core = reinterpret_cast<Udp::Core*>(handle->data);
    //从kcp实现来看其udp包不可以用超过kcp->mtu。所以最大的缓冲区大小可为Core::kMTU。
    //在调整kcp->mtu值，也不应超过Core::kMTU。
    buf->base = core->recv_cache_;
    buf->len = Core::kMTU;
  }
  static void on_recv_read_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
    const struct sockaddr* addr, unsigned flags) {
    Udp::Core* core = reinterpret_cast<Udp::Core*>(handle->data);
    if (nread <= 0) {
      LOG(WARNING) << "udp read error. err:" << nread << " " << uv_strerror(nread);
      return;
    } else if (flags == UV_UDP_PARTIAL) {
      char host_addr[64] = { 0 };
      to_hostaddr(addr, host_addr);
      LOG(WARNING) << "udp partila. send host:" << host_addr;
      return;
    }
    // 数据格式：32位哈希校验码+kcp包头+数据。
    if (IKCP_OVERHEAD + sizeof(uint32_t) > nread) {
      LOG(WARNING) << "ikcp_overhead() > nread:" << nread;
      return;
    }
    // 数据校验
    const uint8_t* ptr = reinterpret_cast<uint8_t*>(buf->base);
    uint32_t packet_hash = 0;
    ptr = _read_u32(ptr, &packet_hash);
    nread -= sizeof(uint32_t);
    uint32_t hash = fnv_32a_buf((void*)ptr, nread, FNV1_32A_INIT);
    if (hash != packet_hash) {
      char host_addr[64] = { 0 };
      to_hostaddr(addr, host_addr);
      LOG(WARNING) << "Validation failed. send host:" << host_addr;
      return;
    } else {
      yx::crypt::xor_string(const_cast<uint8_t*>(ptr), nread, kXorKey, 4);
    }
    bool success = false;
    // 通过Udp方式获取convid有两种方式。
    // 1、由客户端发送convid，服务端无条件接受。优点是简单，其第一个包可以由kcp协议去重发。
    // 为解决convid重复问题，一般是源地址对应一个convid（这里会形成一个状态）。
    // 但有个无法克服逻辑bug：当连接建立后，如果客户端ip port不变再发新连接时，convid可能是变化的，服务端无法接受连接（除非服务端重启）。
    // 2、由服务端发送convid，客户端接受。有个握手过程，但能解决上述问题。
    // 握手过程不能通过kcp去承载，原由是第二次握手中，服务端通过kcp发过来包，客户端无法给出ack，由服务端kcp又会不断发包。
    uint32_t convid = ikcp_getconv(ptr);
    auto iter(core->conns_.find(convid));
    if (core->conns_.end() != iter) {
      Udp* udp = iter->second;
      success = core->handle_recv(udp, ptr, nread, addr);
    } else {
      success = core->handle_connect(convid, ptr, nread, addr);
      if (!success) {
        char host_addr[64] = { 0 };
        to_hostaddr(addr, host_addr);
        LOG(ERROR) << "ConvId " << convid << " does not exist. send host:" << host_addr;
      }
    }
    core->TimerStart(kKCPFast);
  }
protected:
  /*
  @func			: handle_connect
  @brief		: 
  */
  bool handle_connect(uint32_t convid, const uint8_t* buf, uint16_t buf_size, const struct sockaddr* addr) {
    if (Core::kListeningConv == convid) {
      Udp* udp = Spawn(this, reinterpret_cast<const sockaddr_all*>(addr));
      // 简单点，原数据返回。
      udp->Send(buf + IKCP_OVERHEAD, buf_size - IKCP_OVERHEAD);
      delegate_->OnUdpAccpet(nullptr, udp, 0);
      return true;
    } else {
      // 替换发起连接端的老convid。by ZC. 2016-12-27.
      const uint8_t* ptr = buf + IKCP_OVERHEAD;
      uint32_t old_convid = 0;
      ptr = _read_u32(ptr, &old_convid);
      auto iter(conns_.find(old_convid));
      if (conns_.end() == iter)
        return false;
      Udp* connecting_udp = iter->second;
      connecting_udp->kcp_->conv = convid;
      conns_.erase(iter);
      // 最后再将数据交给kcp协议去处理，如发送ack等待。
      if (0 != ikcp_input(connecting_udp->kcp_, (char*)buf, buf_size)) {
        delete connecting_udp;
        return false;
      } else {
        register_udp(convid, connecting_udp);
        connecting_udp->status_ = EStatus::connected;
      }
      // 须取出第二个连接包，可继续做校验。
      char xx[kMTU] = { 0 };
      int packet_size = ikcp_peeksize(connecting_udp->kcp_);
      ikcp_recv(connecting_udp->kcp_, xx, packet_size);
      delegate_->OnUdpConnect(connecting_udp, 0);
      return true;
    }
  }
  /*
  @func			: handle_recv
  @brief		:
  */
  bool handle_recv(Udp* udp, const uint8_t* buf, uint16_t buf_size, const struct sockaddr* addr) {
    IKCPCB* kcp = udp->kcp_;
    if (0 != ikcp_input(kcp, (const char*)buf, buf_size))
      return false;

    int packet_size = 0;
    while (packet_size = ikcp_peeksize(kcp), packet_size > 0) {
      yx::Packet packet(packet_size);
      if (ikcp_recv(kcp, (char*)packet.mutable_buf(), packet_size) < 0)
        break;
      delegate_->OnUdpRead(udp, packet);
      // next
    }
    return true;
  }
protected:
  static void on_timer_cb(uv_timer_t* handle) {
    Core* core = reinterpret_cast<Core*>(handle->data);
    core->handle_timer();
  }
  void handle_timer() {
    uint32_t kcp_now = static_cast<uint32_t>(loop_->ticks_now());
    uint32_t kcp_min_check = 0xFFFFFFFF;
    for (auto iter(conns_.begin()), iterEnd(conns_.end()); iterEnd != iter; ++iter) {
      IKCPCB* kcp = iter->second->kcp_;
      ikcp_update(kcp, kcp_now);
      uint32_t kcp_check = ikcp_check(kcp, kcp_now) - kcp_now;
      if (kcp_check < kcp_min_check)
        kcp_min_check = kcp_check;
    }
    timer_due_ = 0xFFFFFFFFFFFFFFFF;
    TimerStart(kcp_min_check);
  }
private:
  yx::Loop*               loop_;
  IUdpDelegate*           delegate_;
  uv_udp_t udp_;
  uv_timer_t timer_;
  uint64_t   timer_due_;   // 下一次timer的时间
  typedef std::map<uint32_t/*convid*/, Udp*> conns_t;
  conns_t conns_;
  char  recv_cache_[kMTU];
};
//////////////////////////////////////////////////////////////////////////
// Udp
Udp::Udp(Core* core)
  : status_(EStatus::disconnect)
  , core_(core)
  , spawn_(false)
  , kcp_(nullptr)
  , fd_id_(0)
{
  CHECK(core_);
  fd_id_ = core_->loop()->loop_impl()->Register(this);
}

Udp::~Udp()
{
  ikcp_release(kcp_);
}

void* Udp::operator new(size_t sz) {
  return yx_malloc(sz + sizeof(sockaddr_all));
}

void Udp::operator delete(void* ptr) {
  yx_free(ptr);
}

Udp* Udp::New(Loop* loop) {
  Core* core = new Udp::Core(loop);
  return new Udp(core);
}

Udp* Udp::Spawn(Core* core, const sockaddr_all* addr) {
  CHECK(addr && (addr->s.sa_family == AF_INET || addr->s.sa_family == AF_INET6));
  //
  Udp* udp = new Udp(core);
  udp->spawn_ = true;
  if (AF_INET == addr->s.sa_family) {
    memcpy(udp->addr(), addr, sizeof(addr->v4));
  } else if (AF_INET6 == addr->s.sa_family) {
    memcpy(udp->addr(), addr, sizeof(addr->v6));
  } else {
    delete udp;
    return nullptr;
  }
  // 创建信道。 
  uint32_t convid = core->alloc_convid();
  udp->kcp_ = ikcp_create(convid, udp);
  ikcp_setoutput(udp->kcp_, &Udp::on_kcp_output_cb);
  ikcp_nodelay(udp->kcp_, 1, Core::kKCPClock, 2, 1);
  core->register_udp(convid, udp);
  //
  return udp;
}

/*
@func			: Bind
@brief		:
*/
int Udp::Bind(const char* host_addr) {
  if (spawn_) return UV_EAI_CANCELED;
  //
  int err = parse_hostaddr(host_addr, addr());
  if (0 != err) return err;
  //
  return core_->Bind(addr());
}

/*
@func			: Listen
@brief		:
*/
int Udp::Listen(const char* host_addr) {
  int err = Bind(host_addr);
  if (0 != err && UV_EADDRINUSE != err) {
    LOG(ERROR) << "Udp::Bind :" << host_addr << " err [" << uv_err_name(err) << "]:" << uv_strerror(err);
    return err;
  }
  //
  status_ = EStatus::listening;
  err = ReadStart();
  LOG(WARNING) << "Udp::Listen :" << host_addr << " err [" << uv_err_name(err) << "]:" << uv_strerror(err);
  //
  return err;
}

/*
@func			: ReadStart
@brief		:
*/
int Udp::ReadStart() {
  if (spawn_) return UV_EAI_CANCELED;
  //
  return core_->ReadStart();
}

/*
@func			: ReadStop
@brief		:
*/
int Udp::ReadStop() {
  if (spawn_) return UV_EAI_CANCELED;
  //
  return core_->ReadStop();
}

int Udp::on_kcp_output_cb(const char *buf, int len, IKCPCB *kcp, void *user) {
  Udp* udp = reinterpret_cast<Udp*>(user);
  //
  return udp->core_->SendTo(udp->addr(), (const uint8_t*)buf, len);
}

/*
@func			: Send
@brief		: 
*/
int Udp::Send(const uint8_t* buf, uint16_t buf_size) {
  int err = ikcp_send(kcp_, (const char*)buf, buf_size);
  if (0 == err) {
    uint32_t kcp_now = static_cast<uint32_t>(core_->loop()->ticks_now());
    uint32_t kcp_next_update_time = ikcp_check(kcp_, kcp_now);
    if (kcp_next_update_time == kcp_now) {
      ikcp_update(kcp_, kcp_next_update_time);
      kcp_next_update_time = ikcp_check(kcp_, kcp_now);
    } else {
      // 当有数据时，我们希望立即发送数据，如果直接调用ikcp_flush()函数，会影响发送ack时钟。by ZC.
      ikcp_flush_snd_buf(kcp_);
    }
    //
    core_->TimerStart(kcp_next_update_time - kcp_now);
  }
  return err;
}

/*
@func			: Connect
@brief		:
*/
int Udp::Connect(const char* host_addr, uint64_t& connect_fd) {
  if (spawn_) return UV_EAI_CANCELED;
  sockaddr_all addr = { 0 };
  int err = parse_hostaddr(host_addr, &addr);
  if (0 != err) return err;
  //
  Udp* udp = Spawn(core_.get(), &addr);
  uint8_t connect_packet[Core::kMTU] = { 0 };
  // 连接时伪装为kListeningConv信道，连接完成后再收服务端分配conv_id。by ZC. 2016-12-27 10:09.
  _write_u32(connect_packet, Core::kListeningConv);
  uint8_t* buf = connect_packet + IKCP_OVERHEAD;
  buf = _write_u32(buf, udp->kcp_->conv);
  buf = _write_str(buf, (const uint8_t*)host_addr, strlen(host_addr));
  err = core_->SendTo(&addr, connect_packet, buf - connect_packet);
  if (0 == err) {
    udp->status_ = EStatus::connecting;
    connect_fd = udp->fd_id();
  } else {
    udp->status_ = EStatus::disconnect;
    connect_fd = 0;
    udp->Close();
  }
  //
  return err;
}

/*
@func			: set_delegate
@brief		:
*/
void Udp::set_delegate(IUdpDelegate* d) {
  core_->set_delegate(d);
}

/*
@func			: Close
@brief		:
*/
void Udp::Close() {
  core_->loop()->loop_impl()->Unregister(fd_id_);
}

}; // namespace yx

// -------------------------------------------------------------------------