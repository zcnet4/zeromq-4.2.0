/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\zmq_server.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 16:56
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "zmq_server.h"
#include "zmq.h"
#include <stdint.h>
#include "gateway_config.h"
#include "yx/packet.h"
#include "gateway.h"
#include "yx/logging.h"
#include "gateway.h"
#include "yx/tcp_delegate.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// ZmqServer::Zmq
class ZmqServer::Zmq 
{
public:
  Zmq(void* zmq_ctx, uint64_t zmq_id)
    : zmq_id_(zmq_id)
  {
    // http://www.cnblogs.com/fengbohello/p/4354989.html
    zmq_socket_ = zmq_socket(zmq_ctx, ZMQ_PAIR);
  }
  ~Zmq()
  {
    zmq_close(zmq_socket_);
  }
public:
  /*
  @func			: 
  @brief		: 
  */
  void connect(const std::string& server) {
#ifdef _DEBUG
    server_ = server;
#endif // _DEBUG
    //http://www.cnblogs.com/fengbohello/p/4231319.html
    if (0 == zmq_connect(zmq_socket_, server.c_str())) {

    } else {

    }
  }
  /*
  @func			: send
  @brief		: 
  */
  bool send(const yx::Packet& packet) {
    zmq_msg_t zmq_msg;
    yx::Packet send_packet(packet);
    zmq_msg_init_data(&zmq_msg, (void*)packet.buf(), packet.buf_size(), free_yx_packet, send_packet.release());
    int sended = zmq_msg_send(&zmq_msg, zmq_socket_, ZMQ_DONTWAIT);
    zmq_msg_close(&zmq_msg);
    if (sended != packet.buf_size()) {
      if (errno != EAGAIN && errno != EINTR) {
        LOG(ERROR) << "Gateway --> GameServer send failed. errno = "
          << errno << " error msg: " << strerror(errno);
      }
      return false;
    }
    //
    return true;
  }
  static void free_yx_packet(void *data, void *hint) {
    yx::Packet packet;
    packet.reset(hint);
  }
  /*
  @func			: 
  @brief		: 
  */
  bool recv(zmq_msg_t* zmq_msg) {
    int rc = zmq_msg_recv(zmq_msg, zmq_socket_, ZMQ_DONTWAIT);
    if (rc == -1) {
      if (errno != EINTR && errno != EAGAIN) {
        LOG(ERROR) << "Gateway recv msg from game server failed. errno = "
          << errno << " error msg: " << strerror(errno);
      }
      return false;
    }
    return true;
  }
  uint64_t zmq_id() const {
    return zmq_id_;
  }
private:
  uint64_t zmq_id_;
  void* zmq_socket_;
  friend class ZmqServer;
#ifdef _DEBUG
  std::string server_;
#endif // _DEBUG
};
//////////////////////////////////////////////////////////////////////////
// ZmqServer
ZmqServer::ZmqServer()
  : op_type_(0)
{

}

ZmqServer::~ZmqServer()
{

}


/*
@func			: Start
@brief		:
*/
void ZmqServer::Start() {
  LOG(WARNING) << "ZmqServer::Start";
  // 从源码看来，一个zmq_ctx_至少对应一下ZMQ IO线程。by ZC. 2016-11-21 11:38
  // 一个zmq_socket对应一个ZMQ Reaper Thread。
  zmq_ctx_ = zmq_ctx_new();
  //
  Config& cfg = Config::Instance();
  uint32_t num = cfg.getZmqServerNum();
  LOG(WARNING) << "zmq_server_num:" << num;
  for (uint32_t i = 0; i < num; ++i) {
    uint32_t world_id = 0;
    const char* server_host = cfg.getZmqServerAndWorldId(i, world_id);
    if (server_host && *server_host) {
      std::unique_ptr<Zmq> end(new ZmqServer::Zmq(zmq_ctx_, world_id));
      end->connect(server_host);
      LOG(WARNING) << "zmq connect " << server_host;
      zmqs_.insert(zmqs_t::value_type(world_id, std::move(end)));
    }
  }
  //
  exit_poll_thread_.store(0);
  poll_thread_ = std::thread(&ZmqServer::poll_thread, this);
}

/*
@func			: Stop
@brief		:
*/
void ZmqServer::Stop() {
  zmqs_t ends;
  ends.swap(zmqs_);
  ends.clear();
  //
  exit_poll_thread_.store(1);
  if (poll_thread_.joinable())
    poll_thread_.join();
  //
  zmq_ctx_destroy(zmq_ctx_);
  //
  LOG(WARNING) << "ZmqServer::Stop";
}

void ZmqServer::poll_impl(bool* need_restart) {
  // 初始化。
  int poll_count = (int)zmqs_.size();
  zmq_pollitem_t* poll_infos = new zmq_pollitem_t[poll_count];
  Zmq**           poll_zmqs = new Zmq*[poll_count];
  int index = 0;
  for (auto iter(zmqs_.begin()), iterEnd(zmqs_.end()); iterEnd != iter; ++iter) {
    zmq_pollitem_t& item = poll_infos[index];
    Zmq*& z = poll_zmqs[index];
    z = iter->second.get();
    item.socket = z->zmq_socket_;
    item.events = ZMQ_POLLIN | ZMQ_POLLERR;
    item.revents = 0;
    //[M]linux在64位，int存不下一个指针值。review by zc.
    static_assert(sizeof(item.fd) == sizeof(uint32_t), "sizeof(item.fd) != sizeof(uint32_t)");
    //
    ++index;
  } // for
  //
  if (poll_count == 0) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    *need_restart = true;
    return;
  } else {
    *need_restart = false;
  }
  // poll
  while (!exit_poll_thread_) {
    int rc = zmq_poll(poll_infos, poll_count, 3000);
    if (rc > 0) {
      for (int i = 0, readed = 0; i < poll_count; ++i) {
        zmq_pollitem_t& item = poll_infos[i];
        Zmq*& z = poll_zmqs[i];
        if (item.revents & ZMQ_POLLIN) {
          ++readed;
          if (!Recv(z)) {
            LOG(ERROR) << "zmq poll recv message error";
          }
        } else if (item.revents & ZMQ_POLLERR) {
          ++readed;
        }
        if (rc == readed) {
          break;
        }
      } // for
    } else if (rc < 0) {
      int err = zmq_errno();
      switch (err) {
      case ETERM:
        LOG(ERROR) << "zmq poll error:ETERM";
        break;
      case EFAULT:
        LOG(ERROR) << "zmq poll error:EFAULT";
        break;
      case EINTR:
        LOG(ERROR) << "zmq poll error:EINTR";
        break;
      case ENOTSOCK:
        *need_restart = true;
        LOG(ERROR) << "zmq poll error:ENOTSOCK";
        break;
      default:
        LOG(ERROR) << "zmq poll error:" << err;
      }
      break;
    } else {
      // 超时
    }
  } // while
  //
  delete[] poll_infos;
  delete[] poll_zmqs;
}

/*
@func			: poll_thread
@brief		:
*/
void ZmqServer::poll_thread()
{
  bool need_restart = false;
  do {
    poll_impl(&need_restart);
    //
  } while (!exit_poll_thread_ && need_restart);
}

/*
@func			: Send
@brief		:
*/
void ZmqServer::Send(uint64_t zmq_id, const yx::Packet& packet) {
  auto iter = zmqs_.find(zmq_id);
  if (zmqs_.end() != iter) {
    iter->second->send(packet);
  } else {
    LOG(ERROR) << "can not find ZmqClient bound to worldId " << zmq_id << ", drop msg";
  }
}

void ZmqServer::set_op_type(uint32_t op_type) {
  op_type_ = op_type & ~yx::kTCP_MASK;
}

#ifdef YX_YW
#include "pbc.h"
#include "gateway_constants.h"
#include "gateway_util.h"
extern pbc_env* g_yw_server_pbc_env;
#endif // YX_YW

bool ZmqServer::Recv(ZmqServer::Zmq* z) {
  bool success = false;
  zmq_msg_t zmq_msg;
  zmq_msg_init(&zmq_msg);
  if (z->recv(&zmq_msg)) {
    size_t buf_size = zmq_msg_size(&zmq_msg);
    void* data = zmq_msg_data(&zmq_msg);
    //
#ifndef YX_YW
    yx::Packet packet(buf_size);
    memcpy(packet.mutable_buf(), data, buf_size);
    InputToMain(op_type_, z->zmq_id(), packet);
    success = true;
#else
    // 为了少一次内存分配，为YW开个方便之门。by ZC。2016-12-9 17:44.
    pbc_slice slice;
    slice.buffer = data;
    slice.len = buf_size;
    //
    pbc_rmessage* queued_msg = pbc_rmessage_new(g_yw_server_pbc_env, kPROTO_QueuedMsg, &slice);
    if (queued_msg) {
      uint32_t uid = pbc_rmessage_integer(queued_msg, kPROTO_QueuedMsgUid, 0, nullptr);
      uint32_t type = pbc_rmessage_integer(queued_msg, kPROTO_QueuedMsgType, 0, nullptr);
      int data_size = 0;
      const char* data = pbc_rmessage_string(queued_msg, kPROTO_QueuedMsgData, 0, &data_size);
      //
      yx::Packet packet(sizeof(uint32_t) + sizeof(uint32_t) + data_size);
      uint8_t* buf = yx::_write_u32(packet.mutable_buf(), uid);
      buf = yx::_write_u32(buf, type);
      memcpy(buf, data, data_size);
      //
      InputToMain(op_type_, z->zmq_id(), packet);
      success = true;
      //
      pbc_rmessage_delete(queued_msg);
    } else {
      LOG(ERROR) << "Gateway recv msg from game server parse protocolbuffers failed.";
    }
#endif // YX_YW
  }
  zmq_msg_close(&zmq_msg);
  //
  return success;
}

// -------------------------------------------------------------------------
