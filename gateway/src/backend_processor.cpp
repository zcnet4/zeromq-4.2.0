/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\backend_processor.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 14:26
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "backend_processor.h"
#include "gateway_config.h"
#include "gateway_util.h"
#include "yx/noncopyable.h"
#include "runner.h"
#include "op_defs.h"
#include "yx/logging.h"
#include "gateway.h"
#include "rapidjson/rapidjson.hpp"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
static yx::Processor::Handle kBackendProcessor;
void InputToBackend(uint16_t op_type, uint64_t id, const yx::Packet& packet) {
  kBackendProcessor.Send(yx::kTCP_INPUT | (op_type & ~yx::kTCP_MASK), id, packet);
}
//////////////////////////////////////////////////////////////////////////
// BackendProcessor，后端处理器。与后端服务保持连接。
BackendProcessor::BackendProcessor()
  : runner_(nullptr)
{

}

BackendProcessor::~BackendProcessor()
{

}

/*
@func			: Start
@brief		:
*/
bool BackendProcessor::Start() {
  yx::Processor::Start();
  kBackendProcessor = this->handle();
  //
  skynet_server_.Start(socket_manager());
  //
  Config& cfg = Config::Instance();
  const char* backend_host = cfg.getBackendHost();
  if (backend_host) {
    socket_manager()->TcpListen(backend_host, (TCP_OP::YX_S2B));
    LOG(WARNING) << "backend_host listen:" << backend_host;
  } else {
    LOG(WARNING) << "backend_host did not listen";
  }
#ifdef YX_GF
  forward_server_.Start(socket_manager());
#endif // YX_GF
  //
#ifdef YX_YW
  zmq_server_.set_op_type(yx::kTCP_INPUT | (TCP_OP::YW_B2M));
  zmq_server_.Start();
#endif // YX_YW
  //
  return true;
}

/*
@func			: ProcessTcpData
@brief		:
*/
void BackendProcessor::ProcessTcpData(uint64_t fd, uint16_t op, yx::Packet& packet) {
  switch (op) {
  case TCP_OP::FORWARD_SERVER:
    forward_server_.HandleData(fd, packet);
    break;
  case TCP_OP::SKYNET_SERVER:
    skynet_server_.HandleData(fd, packet);
    break;
  default:
    LOG(WARNING) << "Unrecognized Type:" << op;
  }
}

/*
@func			: ProcessTcpConnect
@brief		:
*/
void BackendProcessor::ProcessTcpConnect(uint64_t fd, uint16_t op, yx::Packet& packet) {
  switch (op) {
  case TCP_OP::FORWARD_SERVER:
    forward_server_.HandleConnect(fd, packet);
    break;
  case TCP_OP::SKYNET_SERVER:
    skynet_server_.HandleConnect(fd, packet);
    break;
  }
}

/*
@func			: ProcessTcpClose
@brief		:
*/
void BackendProcessor::ProcessTcpClose(uint64_t fd, uint16_t op, yx::Packet& packet) {
  switch (op) {
  case TCP_OP::FORWARD_SERVER:
    forward_server_.HandleClose(fd, packet);
    break;
  case TCP_OP::SKYNET_SERVER:
    skynet_server_.HandleClose(fd, packet);
    break;
  }
}

/*
@func			: Stop
@brief		:
*/
void BackendProcessor::Stop() {
#ifdef YX_YW
  zmq_server_.Stop();
#endif // YX_YW
#ifdef YX_GF
  forward_server_.Stop();
#endif // YX_GF
  skynet_server_.Stop();
  //
  yx::Processor::Stop();
  //
  LOG(WARNING) << "BackendProcessor::Stop";
}

/*
@func			: ProcessTcpInput
@brief		:
*/
void BackendProcessor::ProcessTcpInput(uint16_t op, uint64_t id, yx::Packet& packet) {
  switch (op) {
  case TCP_OP::YW_M2B:
    zmq_server_.Send(id, packet);
    break;
  case TCP_OP::FORWARD_F2B:
    forward_server_.Forward(id, 0, packet);
    break;
  case TCP_OP::FORWARD_F2B_CLOSE:
    forward_server_.ForwardClose(id, packet);
    break;
  case TCP_OP::SKYNET_F2F:
  {
    const uint8_t* buf = packet.buf() + 10;
    JsonBuffer josn_buffer;
    rapidjson::Document json_doc(josn_buffer);
    josn_buffer.Parse((const char*)buf, packet.buf_size() - 10, json_doc);
    if (json_doc.IsObject()) {
      rapidjson::Document::ValueType& proxy_tcp = json_doc["proxy_tcp"];
      rapidjson::Document::ValueType& proxy_udp = json_doc["proxy_udp"];
      (void)proxy_udp;
      if (proxy_tcp.IsString()) {
        for (uint16_t i = 0, n = json_doc["n"].GetInt(); i < n; ++i) {
          forward_server_.ConnectGateway(proxy_tcp.GetString());
        }
      }
    }
  } break;
  case TCP_OP::ZMQ_SEND: {
    zmq_server_.SendRaw(id, packet);
  } break;
  default:
    break;
  }
}

// -------------------------------------------------------------------------
