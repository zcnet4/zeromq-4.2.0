/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\zmq\main.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-9 10:40
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include <stdlib.h>
#include <string>
#include "yx/logging.h"
#include "yx/glogger_async.h"
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif 
// -------------------------------------------------------------------------

class GuardGLog
{
public:
  GuardGLog(const char* argv0)
  {
    google::InitGoogleLogging(argv0);
    google::SetStderrLogging(google::GLOG_WARNING);
    FLAGS_colorlogtostderr = true;                                  //设置输出到屏幕的日志显示相应颜色
    FLAGS_logbufsecs = 0;                                           //缓冲日志输出，默认为30秒，此处改为立即输出
    FLAGS_max_log_size = 100;                                       // 单位MB
    FLAGS_stop_logging_if_full_disk = true;                         //当磁盘被写满时，停止日志输出
    config_log_dir(argv0);
#ifndef _WIN32
    google::InstallFailureSignalHandler();                          //捕捉 core dumped
    //google::InstallFailureWriter(&SignalHandle);
#endif
    //async_glogger_set(google::GLOG_INFO, google::base::GetLogger(google::GLOG_INFO));
    //google::base::SetLogger(google::GLOG_INFO, (google::base::Logger*)async_glogger_get(google::GLOG_INFO));
  }
  ~GuardGLog()
  {
    google::ShutdownGoogleLogging();
  }
private:
  void config_log_dir(const char* argv0) {
    std::string log_dir(argv0);
#ifdef _WIN32
    const char kSeparators = '\\';
#else
    const char kSeparators = '/';
#endif
    size_t last_backslash = log_dir.rfind(kSeparators, log_dir.size());
    if (last_backslash != std::string::npos) {
      log_dir.erase(last_backslash + 1);
      log_dir.append("gateway_log");
    } else {
      log_dir.append("/gateway_log");
    }
#ifdef _WIN32
    _mkdir(log_dir.c_str());
#else
    mkdir(log_dir.c_str(), S_IRWXU);
#endif    
    google::SetLogDestination(google::GLOG_INFO, (log_dir + "/INFO_").c_str());
    google::SetLogDestination(google::GLOG_WARNING, (log_dir + "/WARNING_").c_str());
    google::SetLogDestination(google::GLOG_ERROR, (log_dir + "/ERROR_").c_str());
    google::SetLogFilenameExtension("gateway-zmq.");
  }
};

#include "zmq.h"
#include <thread>
#include <chrono>
#include "pbc.h"
#include "yx/yx_util.h"
#include "yx/yx_stl_util.h"
#include "build/compiler_specific.h"
//////////////////////////////////////////////////////////////////////////
class ZmqServer
{
public:
  ZmqServer()
  {
    zmq_ctx_ = zmq_ctx_new();
  }
  ~ZmqServer()
  {
    zmq_ctx_destroy(zmq_ctx_);
  }
public:
  bool Start(const char* host) {
    init_pbc_env();
    // http://www.cnblogs.com/fengbohello/p/4354989.html
    zmq_socket_ = zmq_socket(zmq_ctx_, ZMQ_PAIR);
    zmq_bind(zmq_socket_, host && *host ? host : "tcp://127.0.0.1:5555");
    return true;
  }
  void Run() {
    //每秒5帧
    std::chrono::milliseconds msec(200);
    int count = 0;
    while (true) {
      recv();
      //
      std::this_thread::sleep_for(msec);
      if (++count % 10 == 9)
        SendTest();
    };
  }
  void Stop() {
    zmq_close(zmq_socket_);
  }
protected:
  /*
  @func			: init_pbc_env
  @brief		:
  */
  void init_pbc_env() {
    //
    client_pbc_env_ = pbc_new();
    server_pbc_env_ = pbc_new();
    //
    int result = -1;
#ifdef OS_WIN
    const char* client_proto_name = "d:\\yx_code\\yx\\build\\proto_client.pb";
#else
    const char* client_proto_name = "proto_client.pb";
#endif // OS_WIN
    std::vector<uint8_t> client_buf;
    if (yx::util::ReadFileToBuffer(client_proto_name, &client_buf)) {
      pbc_slice slice;
      slice.buffer = (char*)vector_as_array(&client_buf);
      slice.len = client_buf.size();
      result = pbc_register(client_pbc_env_, &slice);
    }
    //
#ifdef OS_WIN
    const char* server_proto_name = "d:\\yx_code\\yx\\build\\proto_server.pb";
#else
    const char* server_proto_name = "proto_server.pb";
#endif
    std::vector<uint8_t> server_buf;
    if (yx::util::ReadFileToBuffer(server_proto_name, &server_buf)) {
      pbc_slice slice;
      slice.buffer = (char*)vector_as_array(&server_buf);
      slice.len = server_buf.size();
      result = pbc_register(server_pbc_env_, &slice);
    }
  }
  bool recv() {
    zmq_msg_t zmq_msg;
    zmq_msg_init(&zmq_msg);
    int rc = zmq_msg_recv(&zmq_msg, zmq_socket_, ZMQ_DONTWAIT);
    if (rc == -1) {
      if (errno != EINTR && errno != EAGAIN) {
        //LOG(ERROR) << "Gateway recv msg from game server failed. errno = "
        //  << errno << " error msg: " << strerror(errno);
      }
      zmq_msg_close(&zmq_msg);
      return false;
    }
    size_t buf_size = zmq_msg_size(&zmq_msg);
    void* buf = zmq_msg_data(&zmq_msg);
    LOG(WARNING) << "recv...";
    {
      pbc_slice slice;
      slice.len = buf_size;
      slice.buffer = buf;
      pbc_rmessage* queued_msg = pbc_rmessage_new(server_pbc_env_, "QueuedMsg", &slice);
      if (queued_msg) {
        uint32_t uid = pbc_rmessage_integer(queued_msg, "uid", 0, nullptr);
        uint32_t type = pbc_rmessage_integer(queued_msg, "type", 0, nullptr);
        LOG(WARNING) << "recv proto uid:" << uid << " type:" << type;
        {
          pbc_slice data;
          data.buffer = (void*)pbc_rmessage_string(queued_msg, "data", 0, &data.len);
          handle_message(uid, type, &data);
        }
        //
        pbc_rmessage_delete(queued_msg);
      } else {
        LOG(ERROR) << "QueuedMsg Proto error";
      }
    }
    //
    zmq_msg_close(&zmq_msg);
    return true;
  }
  void handle_message(uint32_t uid, uint32_t type, pbc_slice* data) {
#define QMT_GAME     1  // 游戏内协议消息转发
#define QMT_ADD_PEER 2  // 连接建立
#define QMT_DEL_PEER 3  // 连接断开
#define QMT_PEER_NOT_EXIST 4  // 连接不存在
#define QMT_PEER_LOGTOU_AND_SAVED 5  // 通知gateway用户已经退出并存盘成功
#define QMT_REPORT_ONLINE_NUM 6 // 通知gateway在线数
#define QMT_INVALID  10  // 无效的消息
    if (QMT_ADD_PEER == type) {
      LOG(WARNING) << "add peer uid:" << uid;
      return;
    }
    pbc_rmessage* pmsg = pbc_rmessage_new(client_pbc_env_, "Message", data);
    if (pmsg) {
      // 转发的数据就原包回复。
      if (QMT_GAME == type) {
        SendProto(uid, type, data);
      }
      pbc_rmessage_delete(pmsg);
    } else {
      LOG(ERROR) << "Message Proto error";
    }
  }

  void SendTest() {
    pbc_wmessage* msg = pbc_wmessage_new(server_pbc_env_, "s2s_online_num");
    if (msg) {
      pbc_wmessage_integer(msg, "world_id", 1, 0);
      pbc_wmessage_integer(msg, "online_num", 1, 0);
      //
      pbc_slice serialize_slice;
      pbc_wmessage_buffer(msg, &serialize_slice);
      SendProto(0, QMT_REPORT_ONLINE_NUM, &serialize_slice);
      //
      pbc_wmessage_delete(msg);
    }
  }

  void SendProto(uint32_t uid, uint32_t type, pbc_slice* slice) {
    LOG(WARNING) << "SendProto uid:" << uid << " type:" << type;
    pbc_wmessage* queued_msg = pbc_wmessage_new(server_pbc_env_, "QueuedMsg");
    if (queued_msg) {
      pbc_wmessage_integer(queued_msg, "uid", uid, 0);
      pbc_wmessage_integer(queued_msg, "type", type, 0);
      if (slice)
        pbc_wmessage_string(queued_msg, "data", (const char*)slice->buffer, slice->len);
      //
      send_impl(queued_msg);
    }
  }

  bool send_impl(pbc_wmessage* queued_msg) {
    pbc_slice serialize_slice;
    pbc_wmessage_buffer(queued_msg, &serialize_slice);
    //
    zmq_msg_t zmq_msg;
    zmq_msg_init_data(&zmq_msg, (void*)serialize_slice.buffer, serialize_slice.len, free_pbc_msg, queued_msg);
    int sended = zmq_msg_send(&zmq_msg, zmq_socket_, ZMQ_DONTWAIT);
    zmq_msg_close(&zmq_msg);
    if (sended != serialize_slice.len) {
      if (errno != EAGAIN && errno != EINTR) {
        LOG(ERROR) << "Gateway --> GameServer send failed. errno = "
          << errno << " error msg: " << strerror(errno);
      }
      return false;
    }
    //
    return true;
  }
  static void free_pbc_msg(void *data, void *hint) {
    pbc_wmessage_delete((pbc_wmessage*)hint);
  }
private:
  void* zmq_ctx_;
  void* zmq_socket_;
  pbc_env* client_pbc_env_;
  pbc_env* server_pbc_env_;
};

//////////////////////////////////////////////////////////////////////////
// main
int main(int argc, const char * argv[]) {
  // 日志
  GuardGLog gurad_glog(argv[0]);
  ZmqServer gate;
  if (gate.Start(argv[1])) {
    gate.Run();
  }
  gate.Stop();
  //
  return 0;
}



// -------------------------------------------------------------------------
