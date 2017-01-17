/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\zmq_server.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 16:56
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef ZMQ_SERVER_H_
#define ZMQ_SERVER_H_
#include <memory>
#include <thread>
#include <unordered_map>
#include <atomic>
// -------------------------------------------------------------------------
namespace yx {
  class Packet;
}
//////////////////////////////////////////////////////////////////////////
// ZmqServer
class ZmqServer
{
public:
  ZmqServer();
  ~ZmqServer();
public:
  /*
  @func			: Start
  @brief		: 
  */
  void Start();
  /*
  @func			: Stop
  @brief		: 
  */
  void Stop();
  /*
  @func			: Send
  @brief		: 
  */
  void Send(uint64_t zmq_id, const yx::Packet& packet);
  void set_op_type(uint32_t op_type);
protected:
  /*
  @func			: poll_impl
  @brief		: 
  */
  void poll_impl(bool* need_restart);
  /*
  @func			: poll_thread
  @brief		:
  */
  void poll_thread();
private:
  void* zmq_ctx_;
  class Zmq;    // ÷’∂À°£
  bool Recv(Zmq* z);
  typedef std::unordered_map<uint64_t, std::unique_ptr<Zmq>>  zmqs_t;
  zmqs_t zmqs_;
  std::thread poll_thread_;
  std::atomic<char> exit_poll_thread_;
  uint32_t op_type_;
};


// -------------------------------------------------------------------------
#endif /* ZMQ_SERVER_H_ */
