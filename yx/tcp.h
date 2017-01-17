/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\tcp.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-26 11:10
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef TCP_H_
#define TCP_H_
#include <stdint.h>
#include "recv_cache.h"
#include "yx_export.h"
#include "tcp_delegate.h"
// -------------------------------------------------------------------------
struct uv_stream_s;
typedef uv_stream_s uv_stream_t;
struct uv_handle_s;
typedef uv_handle_s uv_handle_t;
struct uv_write_s;
typedef uv_write_s uv_write_t;
struct uv_connect_s;
typedef uv_connect_s uv_connect_t;
struct uv_buf_t;
#ifdef _MSC_VER
  #if !defined(_SSIZE_T_) && !defined(_SSIZE_T_DEFINED)
  typedef intptr_t ssize_t;
  # define _SSIZE_T_
  # define _SSIZE_T_DEFINED
  #endif
#else
  #include <sys/types.h> // for ssize_t
#endif // _MSC_VER
//////////////////////////////////////////////////////////////////////////
//
namespace yx {
class Loop;
class Packet;
class PacketView;
struct ITcpDelegate;
//////////////////////////////////////////////////////////////////////////
// Tcp，是uv_tcp_t,uv_pipe_t的封闭类，并具有一些公共逻辑与状态处理类。
class YX_EXPORT Tcp {
public:
  enum class EStatus : uint8_t {
    disconnect = 0,
    listening,
    connecting,
    connected,
    action,
  };
  /*
  @func			: New
  @brief		:
  */
  static Tcp* New(Loop* loop);
  /*
  @func			: NewPipe
  @brief		:
  */
  static Tcp* NewPipe(Loop* loop);
public:
  /*
  @func			: Connect
  @brief		:
  */
  int Connect(const char* host_addr);
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
  @func			: Accpet
  @brief		:
  */
  int Accpet();
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
  @func			: Send
  @brief		: 默认写[len]包长字段
  */
  int Send(const PacketView& v1);
  /*
  @func			: Send2
  @brief		: 默认写[len]包长字段
  */
  int Send2(const PacketView& v1, const PacketView& v2);
  /*
  @func			: Close
  @brief		:
  */
  void Close();
  /*
  @func			: set_delegate
  @brief		:
  */
  void set_delegate(ITcpDelegate* d) {
    delegate_ = d;
  }
  Loop* loop() const { return loop_; }
  uint64_t tcp_id() const { return tcp_id_; }
  uint16_t tcp_type() const { return tcp_type_; }
  void set_tcp_type(uint16_t tcp_type) { tcp_type_ = tcp_type; }
  uint32_t index32() const { return index32_; }
  void set_index32(uint32_t index32) { index32_ = index32; }
  uint64_t recv_time() const { return recv_time_; }
  uint64_t send_time() const { return send_time_; }
  void set_nodelay(bool enable);
  /*
  @func			: get_peerhost
  @brief		: 
  */
  void get_peerhost(char peer_host[32]);
protected:
  static void on_accpet_cb(uv_stream_t* server, int status);
  static void on_close_cb(uv_handle_t* handle);
  static void on_recv_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
  static void on_recv_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
  static void on_write_cb(uv_write_t* req, int status);
  static void on_connect_cb(uv_connect_t* req, int status);
private:
  Tcp(Loop* loop);
  Tcp(const Tcp&) = delete;
  Tcp& operator=(const Tcp&) = delete;
  ~Tcp();
  // 
  void* operator new(size_t) throw();
  void* operator new(size_t, void* ptr){ return ptr; }
  void operator delete(void*);
  uv_stream_t* handle() { return reinterpret_cast<uv_stream_t*>(this + 1); }
private:
  EStatus                   status_;
  uint8_t                   padding_;
  uint16_t                  tcp_type_;
  uint32_t                  index32_;
  Loop*                     loop_;
  ITcpDelegate*             delegate_;
  uint64_t                  tcp_id_;
  uint64_t                  send_time_;
  uint64_t		              recv_time_;
  RecvCache                 recv_cache_;
};

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* TCP_H_ */
