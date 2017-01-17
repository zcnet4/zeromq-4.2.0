/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\frontend_processor.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 14:25
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "frontend_processor.h"
#include "gateway_config.h"
#include "gateway_util.h"
#include "runner.h"
#include "op_defs.h"
#include "netproto.h"
#include "yx/logging.h"
#include "gateway.h"
#include "yx/packet_view.h"
#include "yx/yx_time.h"
#include "yx/udp.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
static yx::Processor::Handle kFrontendProcessor;
void InputToFrontend(uint16_t op_type, uint64_t id, const yx::Packet& packet) {
  kFrontendProcessor.Send(yx::kTCP_INPUT | (op_type & ~yx::kTCP_MASK), id, packet);
}
//////////////////////////////////////////////////////////////////////////
// FrontendManager，前端管理器。与客户端保持连接。
FrontendProcessor::FrontendProcessor()
{
  vtcpid_allocator_.set_worker_id(31);
  vtcpid_allocator_.set_machine_id(yx::GetMachineId());
}

FrontendProcessor::~FrontendProcessor()
{

}

/*
@func			: Start
@brief		:
*/
bool FrontendProcessor::Start() {
  yx::Processor::Start();
  kFrontendProcessor = this->handle();
  //
  Config& cfg = Config::Instance();
  //
  const char* frontend_host = cfg.getFrontendHost();
#ifndef YX_GF
  if (frontend_host) {
    socket_manager()->TcpListen(frontend_host, (TCP_OP::YW_C2F));
    LOG(WARNING) << "frontend_host listen:" << frontend_host;
  } else {
    LOG(WARNING) << "frontend_host did not listen";
  }
  const char* proxy_tcp = cfg.getProxyTcpHost();
  if (proxy_tcp) {
    socket_manager()->TcpListen(proxy_tcp, (TCP_OP::YW_GF2G));
    LOG(WARNING) << "proxy_tcp listen:" << proxy_tcp;
  } else {
    LOG(WARNING) << "proxy_tcp did not listen";
  }
  const char* proxy_udp = cfg.getProxyUdpHost();
  if (proxy_tcp) {
    socket_manager()->UdpListen(proxy_udp, 0);
    LOG(WARNING) << "proxy_udp listen:" << proxy_udp;
  } else {
    LOG(WARNING) << "proxy_udp did not listen";
  }
#else
  if (frontend_host) {
    socket_manager()->TcpListen(frontend_host, (TCP_OP::FORWARD_C2F));
    LOG(WARNING) << "frontend_host listen:" << frontend_host;
  } else {
    LOG(WARNING) << "frontend_host did not listen";
  }
#endif // YX_GF
  const char* pingpong_host = cfg.getPingPongHost();
  if (pingpong_host) {
    socket_manager()->TcpListen(pingpong_host, TCP_OP::PINGPONG_SERVER);
    LOG(WARNING) << "pingpong_host listen:" << pingpong_host;
  }
  //
  return true;
}

/*
@func			: ProcessTcpData
@brief		:
*/
void FrontendProcessor::ProcessTcpData(uint64_t fd, uint16_t op, yx::Packet& packet) {
  switch (op) {
  case TCP_OP::YW_C2F: 
    InputToMain((TCP_OP::YW_F2M), fd, packet);
    break;
  case TCP_OP::FORWARD_C2F:
#ifdef YX_GF
    // gateway-forward直接转发到后端去。
  {
    const uint8_t* buf = packet.buf();
    uint16_t cmd = 0;
    buf = yx::_read_u16(buf, &cmd);
    uint32_t session = 0, uid = 0;
    buf = yx::_read_u32(buf, &session);
    buf = yx::_read_u32(buf, &uid);
    LOG(WARNING) << "recv session:" << session;
  }
    InputToBackend((TCP_OP::FORWARD_F2B), fd, packet);
#endif // YX_GF
    break;
  case TCP_OP::YW_GF2G: {
    const uint8_t* buf = packet.buf();
    uint16_t cmd = 0;
    buf = yx::_read_u16(buf, &cmd);
    uint32_t session = 0, uid = 0;
    buf = yx::_read_u32(buf, &session);
    buf = yx::_read_u32(buf, &uid);
    if (static_cast<uint16_t>(NETCMD_G2G::GF2G_FORWARD) == cmd) {
      uint64_t forward_id = uid;
      forward_id <<= 32;
      forward_id |= session;
      uint64_t vtcp_id = OpenVTcpId(forward_id, true);
      // set offset
      packet.set_param(buf - packet.buf());
      InputToMain((TCP_OP::YW_F2M), vtcp_id, packet);
    }
    else if (static_cast<uint16_t>(NETCMD_G2G::GF2G_FORWARD_CLOSE) == cmd) {
      uint64_t forward_id = uid;
      forward_id <<= 32;
      forward_id |= session;
      // set offset
      packet.set_param(buf - packet.buf());
      //
      uint64_t vtcp_id = OpenVTcpId(forward_id, false);
      SendToMain(vtcp_id, yx::kTCP_CLOSE | (TCP_OP::YW_C2F), packet);
      //
      CloseVTcpId(forward_id);
    }
  } break;
  case TCP_OP::PINGPONG_SERVER: {
    socket_manager()->Send(fd, packet);
  } break;
  default:
    LOG(WARNING) << "Unrecognized Type:" << op;
  }
}

/*
@func			: Stop
@brief		:
*/
void FrontendProcessor::Stop() {
  yx::Processor::Stop();
  //
  LOG(WARNING) << "FrontendProcessor::Stop";
}

/*
@func			: ProcessTcpInput
@brief		:
*/
void FrontendProcessor::ProcessTcpInput(uint16_t op, uint64_t id, yx::Packet& packet) {
  switch (op) {
  case TCP_OP::FORWARD_B2F: {
    uint64_t tcp_id = id;
    if ((sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t)) == packet.param()) {
      yx::PacketView view(packet);
      view.inset(packet.offset());
      socket_manager()->Send(tcp_id, view);
    }
    else
      LOG(ERROR) << "Error PacketOp::FORWARD_B2F";
  } break;
  case TCP_OP::YW_M2F: {
      uint64_t tcp_id = id;
      yx::PacketView view(packet);
      view.inset(packet.offset());
      socket_manager()->Send(tcp_id, view);
  } break;
  case TCP_OP::YW_G2GF: {
    uint64_t tcp_id = id;
    yx::PacketView v1;
    yx::PacketView v2(packet);
    v2.inset(packet.offset());
    socket_manager()->Send2(tcp_id, v1, v2);
  } break;
  }
}

/*
@func			: ProcessTcpConnect
@brief		:
*/
void FrontendProcessor::ProcessTcpConnect(uint64_t fd, uint16_t op, yx::Packet& packet) {

}

/*
@func			: ProcessTcpClose
@brief		:
*/
void FrontendProcessor::ProcessTcpClose(uint64_t fd, uint16_t op, yx::Packet& packet) {
  switch (op) {
  case TCP_OP::FORWARD_C2F:
#ifdef YX_GF
    // gateway-forward直接转发到后端去。
    InputToBackend((TCP_OP::FORWARD_F2B_CLOSE), fd, packet);
#endif // YX_GF
    break;
  case TCP_OP::YW_C2F:
    SendToMain(fd, yx::kTCP_CLOSE | op, packet);
    break;
  }
}


/*
@func			: GetOrCreateVTcpId
@brief		:
*/
uint64_t FrontendProcessor::OpenVTcpId(uint64_t forward_id, bool auto_create/* = true*/) {
  id2id_t::iterator iter(forward_ids_.find(forward_id));
  if (forward_ids_.end() != iter) {
    // vtcp_id
    return iter->second;
  }
  uint64_t vtcp_id = 0;

  if (auto_create) {
    vtcp_id = vtcpid_allocator_.generate(loop()->time_now());
    forward_ids_.emplace(id2id_t::value_type(forward_id, vtcp_id));
    vtcp_ids_.emplace(id2id_t::value_type(vtcp_id, forward_id));
  }
  //
  return vtcp_id;
}

/*
@func			: CloseVTcpId
@brief		:
*/
void FrontendProcessor::CloseVTcpId(uint64_t forward_id) {
  id2id_t::iterator iter(forward_ids_.find(forward_id));
  if (forward_ids_.end() == iter) return;
  //
  uint64_t vtcp_id = iter->second;
  forward_ids_.erase(iter);
  //
  iter = vtcp_ids_.find(vtcp_id);
  if (vtcp_ids_.end() != iter)
    vtcp_ids_.erase(iter);
}
// -------------------------------------------------------------------------
