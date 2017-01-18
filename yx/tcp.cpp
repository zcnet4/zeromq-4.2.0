/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\tcp.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-26 11:10
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "tcp.h"
#include "uv.h"
#include "loop.h"
#include "loop_impl.h"
#include "tcp_delegate.h"
#include "packet_view.h"
#include "logging.h"
#include "allocator.h"
#include "_sockaddr.h"
#include "socket_manager.h"
// -------------------------------------------------------------------------
namespace yx {

#define BACKLOG 1024
//////////////////////////////////////////////////////////////////////////
// Tcp
Tcp::Tcp(Loop* loop)
  : status_(EStatus::disconnect)
  , padding_(0)
  , tcp_type_(0)
  , index32_(0)
  , loop_(loop)
  , delegate_(nullptr)
  , tcp_id_(0)
  , send_time_(0)
  , recv_time_(0)
{
  handle()->data = this;
  tcp_id_ = loop->loop_impl()->Register(this);
}

Tcp::~Tcp()
{
  recv_cache_.clear(loop_->loop_impl()->recv_cache_pool());
}

void* Tcp::operator new(size_t sz) throw() {
  CHECK(false);
  return nullptr;
}

void Tcp::operator delete(void* ptr) {
  yx_free(ptr);
}

/*
@func			: New
@brief		:
*/
Tcp* Tcp::New(Loop* loop) {
  CHECK(loop);
  //
  Tcp* tcp = (Tcp*)yx_malloc(sizeof(Tcp) + sizeof(uv_tcp_t));
  int err = uv_tcp_init(loop->uv_loop(), reinterpret_cast<uv_tcp_t*>(tcp + 1));
  if (0 != err) {
    yx_free(tcp);
    return nullptr;
  }
  //
  ::new (tcp) Tcp(loop);
  return tcp;
}

/*
@func			: NewPipe
@brief		:
*/
Tcp* Tcp::NewPipe(Loop* loop) {
  CHECK(loop);
  //
  Tcp* tcp = (Tcp*)yx_malloc(sizeof(Tcp) + sizeof(uv_pipe_t));
  int err = uv_pipe_init(loop->uv_loop(), reinterpret_cast<uv_pipe_t*>(tcp + 1), 0);
  if (0 != err) {
    yx_free(tcp);
    return nullptr;
  }
  //
  ::new (tcp) Tcp(loop);
  return tcp;
}

void Tcp::on_connect_cb(uv_connect_t* req, int status) {
  Tcp* tcp = (Tcp*)req->data;
  tcp->status_ = EStatus::connected;
  tcp->delegate_->OnTcpConnect(tcp, status);
  //
  LoopImpl::free_uv_req(req);
}

/*
@func			: Connect
@brief		:
*/
int Tcp::Connect(const char* host_addr) {
  if (UV_TCP == handle()->type) {
    sockaddr_all connect_addr = { 0 };
    int err = parse_hostaddr(host_addr, &connect_addr);
    if (0 != err) {
      LOG(WARNING) << "Wrong IP address:" << host_addr;
      return err;
    }
    //
    uv_connect_t* req = (uv_connect_t*)loop_->loop_impl()->alloc_uv_req();
    req->data = this;
    status_ = EStatus::connecting;
    err = uv_tcp_connect(req, (uv_tcp_t*)handle(), &connect_addr.s, &Tcp::on_connect_cb);
    //
    return err;
  } else if (UV_NAMED_PIPE == handle()->type) {
    uv_connect_t* req = (uv_connect_t*)loop_->loop_impl()->alloc_uv_req();
    req->data = this;
    status_ = EStatus::connecting;
    uv_pipe_connect(req, (uv_pipe_t*)handle(), host_addr, &Tcp::on_connect_cb);
    //
    return 0;
  }
  return -1;
}

/*
@func			: Bind
@brief		:
*/
int Tcp::Bind(const char* host_addr) {
  if (UV_TCP == handle()->type) {
    sockaddr_all listen_addr = { 0 };
    int err = parse_hostaddr(host_addr, &listen_addr);
    if (0 != err) {
      LOG(WARNING) << "Wrong IP address:" << host_addr;
      return err;
    }
    //
    err = uv_tcp_bind((uv_tcp_t*)handle(), &listen_addr.s, 0);
    //
    return err;
  } else if (UV_NAMED_PIPE == handle()->type) {
    int err = uv_pipe_bind((uv_pipe_t*)handle(), host_addr);
    //
    return err;
  }
  return -1;
}

void Tcp::on_accpet_cb(uv_stream_t* server, int status) {
  Tcp* tcp = (Tcp*)server->data;
  if (0 == status) {
    tcp->Accpet();
  }
}

/*
@func			: Listen
@brief		:
*/
int Tcp::Listen(const char* host_addr) {
  int err = Bind(host_addr);
  if (0 != err && UV_EADDRINUSE != err) {
    LOG(ERROR) << "Tcp::Bind :" << host_addr << " err [" << uv_err_name(err) <<"]:" << uv_strerror(err);
    return err;
  }
  //
  err = uv_listen(handle(), BACKLOG, Tcp::on_accpet_cb);
  status_ = EStatus::listening;
  LOG(WARNING) << "Tcp::Listen :" << host_addr << " err [" << uv_err_name(err) << "]:" << uv_strerror(err);
  //
  return err;
}

/*
@func			: Accpet
@brief		:
*/
int Tcp::Accpet() {
	Tcp* accpet_tcp = nullptr;
	switch (handle()->type)
	{
	case UV_TCP:
		accpet_tcp = Tcp::New(loop());
		break;
	case UV_NAMED_PIPE:
		accpet_tcp = Tcp::NewPipe(loop());
	default:
		break;
	}
	int err = uv_accept(handle(), accpet_tcp->handle());
	status_ = EStatus::connected;
	//
  delegate_->OnTcpAccpet(this, accpet_tcp, err);
	//
	return err;
}

void Tcp::on_close_cb(uv_handle_t* handle) {
  Tcp* tcp = (Tcp*)handle->data;
  //
  tcp->delegate_ = nullptr;
  handle->data = nullptr;
  //
  delete tcp;
}

/*
@func			: Close
@brief		:
*/
void Tcp::Close() {
  char peer_host[32];
  get_peerhost(peer_host);
  LOG(INFO) << "Close client from : " << peer_host;
  //
  status_ = EStatus::disconnect;
  if (delegate_) delegate_->OnTcpClose(this);
  //
  loop()->loop_impl()->Unregister(tcp_id());
  //
  uv_close((uv_handle_t*)handle(), &Tcp::on_close_cb);
}

void Tcp::on_recv_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  Tcp* tcp = (Tcp*)handle->data;
  LoopImpl* loop_impl = tcp->loop()->loop_impl();
  //
  tcp->recv_cache_.alloc(loop_impl->recv_cache_pool(), buf);
}

void Tcp::on_recv_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  Tcp* tcp = (Tcp*)stream->data;
  Loop* loop = tcp->loop();
  //
  if (nread > 0) {
    MemoryPool* pool = loop->loop_impl()->recv_cache_pool();
    RecvCache& recv_cache = tcp->recv_cache_;
    recv_cache.recv_size(nread);
    //
    uint16_t packet_size = 0;
    while (recv_cache.decode_header(pool, &packet_size)) {
      // 记录最近接收数据时间。      tcp->recv_time_ = loop->ticks_now();
      //
      yx::Packet packet(packet_size);
      recv_cache.decode(pool, packet.mutable_buf(), packet_size);
      tcp->delegate_->OnTcpRead(tcp, packet);
    }
  } else if (nread < 0) { /* Error or EOF */
    tcp->Close();
  } else {
    /* Everything OK, but nothing read. */
    tcp->recv_time_ = loop->ticks_now();
  }
}

/*
@func			: ReadStart
@brief		:
*/
int Tcp::ReadStart() {
  return uv_read_start(handle(), &Tcp::on_recv_alloc_cb, &Tcp::on_recv_read_cb);
}

/*
@func			: ReadStop
@brief		:
*/
int Tcp::ReadStop() {
  return uv_read_stop(handle());
}


static char* align_ptr(char* ptr) {
  std::size_t alignment = ((sizeof(void *) - (std::size_t(ptr) & (sizeof(void *) - 1))) & (sizeof(void *) - 1));
  return ptr + alignment;
}

static char* write_len(uv_write_t* req, uint16_t len, uv_buf_t* buf) {
  buf->base = align_ptr(reinterpret_cast<char*>(req + 1));
  buf->len = sizeof(uint16_t);
  buf->base[0] = (len >> 8) & 0xff;
  buf->base[1] = len & 0xff;
  return  buf->base + sizeof(uint16_t);
}

void Tcp::on_write_cb(uv_write_t* req, int status) {
  yx::PacketView* obj_buf = reinterpret_cast<yx::PacketView*>(align_ptr(reinterpret_cast<char*>(req + 1)) + sizeof(uint16_t));
  for (uint64_t i = 0, count = reinterpret_cast<uint64_t>(req->data); i < count; ++i) {
    (obj_buf + i )->~PacketView();
  }
  LoopImpl::free_uv_req(req);
  // 如果写失败，则关闭Close。
  if (status != 0) {
    LOG(ERROR) << "Tcp write error:" << status;
    Tcp* tcp = reinterpret_cast<Tcp*>(req->handle->data);
    LOG(ERROR) << "Tcp write failure is turned off. tcp_id:" << tcp->tcp_id();
    tcp->Close();
  }
}

/*
@func			: Send
@brief		: 默认写[len]包长字段
*/
int Tcp::Send(const PacketView& v1) {
  uv_buf_t buf[2];         
  buf[1] = uv_buf_init((char*)v1.buf(), v1.buf_size());
  // 优化write req分配。
  uv_write_t* req = reinterpret_cast<uv_write_t*>(loop_->loop_impl()->alloc_uv_req());
  //
#ifdef PACKET_SIZE_CONTAINS_PACKET_SIZE
  yx::PacketView* obj_buf = (yx::PacketView*)write_len(req, sizeof(uint16_t) + v1.buf_size(), &buf[0]);
#else
  yx::PacketView* obj_buf = (yx::PacketView*)write_len(req, v1.buf_size(), &buf[0]);
#endif // PACKET_SIZE_CONTAINS_PACKET_SIZE
  ::new (obj_buf)yx::PacketView(v1);
  req->data = (uint64_t*)1;
  //
  send_time_ = loop_->ticks_now();
  return uv_write(req, handle(), buf, 2, on_write_cb);
}

/*
@func			: Send2
@brief		: 默认写[len]包长字段
*/
int Tcp::Send2(const PacketView& v1, const PacketView& v2) {
  uv_buf_t buf[3];
  buf[1] = uv_buf_init((char*)v1.buf(), v1.buf_size());
  buf[2] = uv_buf_init((char*)v2.buf(), v2.buf_size());
  // 优化write req分配。
  uv_write_t* req = reinterpret_cast<uv_write_t*>(loop_->loop_impl()->alloc_uv_req());
  //
#ifdef PACKET_SIZE_CONTAINS_PACKET_SIZE
  yx::PacketView* obj_buf = (yx::PacketView*)write_len(req, sizeof(uint16_t) + v1.buf_size() + v2.buf_size(), &buf[0]);
#else
  yx::PacketView* obj_buf = (yx::PacketView*)write_len(req, v1.buf_size() + v2.buf_size(), &buf[0]);
#endif // PACKET_SIZE_CONTAINS_PACKET_SIZE
  ::new (obj_buf)yx::PacketView(v1);
  ::new (obj_buf + 1) yx::PacketView(v2);
  req->data = (uint64_t*)2;
  //
  send_time_ = loop_->ticks_now();
  return uv_write(req, handle(), buf, 3, on_write_cb);
}

void Tcp::set_nodelay(bool enable) {
  if (UV_TCP == handle()->type) {
    uv_tcp_nodelay(reinterpret_cast<uv_tcp_t*>(handle()), enable);
  }
}

/*
@func			: get_peerhost
@brief		:
*/
void Tcp::get_peerhost(char peer_host[32]) {
  if (UV_TCP == handle()->type) {
    struct sockaddr_in peer_addr;
    int namelen = sizeof(peer_addr);
    uv_tcp_getpeername((uv_tcp_t*)handle(), (sockaddr*)&peer_addr, &namelen);
    uv_ip4_name(&peer_addr, peer_host, 32);
    char* buf = peer_host + strlen(peer_host);
#ifdef _MSC_VER
#define snprintf _snprintf
#endif
    snprintf(buf, buf - peer_host, ":%d", peer_addr.sin_port);
  }
}

}; // namespace yx


// -------------------------------------------------------------------------
