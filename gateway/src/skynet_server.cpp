/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\skynet_server.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-29 11:28
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "skynet_server.h"
#include "gateway_config.h"
#include "op_defs.h"
#include "yx/socket_manager.h"
#include "yx/logging.h"
#include "yx/crypt.h"
#include "netproto.h"
#include "gateway_util.h"
#include "gateway.h"
#include <chrono>
#include <thread>
#include "rapidjson/rapidjson.hpp"
#include "yx/yx_timer.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// SkynetServer::connect_ap 
struct SkynetServer::AP
{
  uint8_t       ckey_[8];
  uint8_t       secret_[8];
  uint16_t      state;
  uint64_t      tcp_id;
  char          server_[64];
  char          addr_[64];

  bool handle_connect(yx::Packet& out_packet) {
    yx::crypt::randomkey(ckey_);
    uint8_t ckey_change[8] = { 0 };
    yx::crypt::dhexchange(ckey_, ckey_change);
    //
    yx::Packet connect_packet(10 + sizeof(ckey_change));
    uint8_t* buf = connect_packet.mutable_buf();
    // len, cmd, session, uid
    buf = yx::_write_u16(buf, static_cast<uint16_t>(NETCMD_C2S::CONNECT));
    buf = yx::_write_u32(buf, 0);
    buf = yx::_write_u32(buf, 0);
    //
    memcpy(buf, ckey_change, sizeof(ckey_change));
    //
    state = static_cast<uint16_t>(NETCMD_C2S::CONNECT);
    out_packet = connect_packet;
    return true;
  }
  bool handle_challenge(const uint8_t* buf, uint16_t buf_size, yx::Packet& out_packet) {
    if (buf_size >= 8 + 8) {
      uint8_t challenge[8] = { 0 };
      memcpy(challenge, buf, sizeof(challenge));
      uint8_t skey_change[8] = { 0 };
      memcpy(skey_change, buf + 8, sizeof(skey_change));
      //
      yx::crypt::dhsecret(skey_change, ckey_, secret_);
      uint16_t server_size = strlen(server_);
      //
      yx::Packet packet(10 + sizeof(uint8_t)*8 + server_size + 1);
      uint8_t* buf = packet.mutable_buf();
      // len, cmd, session, uid
      buf = yx::_write_u16(buf, static_cast<uint16_t>(NETCMD_C2S::CHALLENGE_RESPONSE));
      buf = yx::_write_u32(buf, 0);
      buf = yx::_write_u32(buf, 0);
      // hmac64
      yx::crypt::hmac64(challenge, secret_, buf);
      buf += sizeof(uint8_t) * 8;
      //
      memcpy(buf, server_, server_size);
      yx::crypt::xor_string((uint8_t*)buf, server_size, (const uint8_t*)secret_, sizeof(secret_));
      buf += server_size;
      *buf = '\0';
      //
      out_packet = packet;
      return true;
    }
    return false;
  }
  void join_gateway_manager(yx::Packet& p) {
    Config& cfg = Config::Instance();
    JsonBuffer json_buffer;
    rapidjson::Writer<JsonBuffer> json_writer(json_buffer, json_buffer);
    json_writer.StartObject();
    json_writer.String("join");
    json_writer.StartObject();
    json_writer.String("name").String(cfg.getGatewayName());
    json_writer.String("pwd").String(cfg.getGatewayPWD());
    json_writer.String("mid").Uint(cfg.getMachineId());
    json_writer.String("frontend_host");
    if (cfg.getFrontendHost()) {
      json_writer.String(cfg.getFrontendHost());
    } else {
      json_writer.Null();
    }
    json_writer.String("backend_host");
    if (cfg.getBackendHost()) {
      json_writer.String(cfg.getBackendHost());
    } else {
      json_writer.Null();
    }
    json_writer.String("proxy_tcp");
    if (cfg.getProxyTcpHost()) {
      json_writer.String(cfg.getProxyTcpHost());
    } else {
      json_writer.Null();
    }
    json_writer.String("proxy_udp");
    if (cfg.getProxyUdpHost()) {
      json_writer.String(cfg.getProxyUdpHost());
    } else {
      json_writer.Null();
    }
    json_writer.EndObject();
    json_writer.EndObject();
    //
    yx::Packet packet(10 + json_buffer.Size());
    // len, cmd, session, uid
    uint8_t* buf = yx::_write_u16(packet.mutable_buf(), static_cast<uint16_t>(NETCMD_C2S::JOIN_SERVER));
    buf = yx::_write_u32(buf, 0);
    buf = yx::_write_u32(buf, 0);
    //
    memcpy(buf, json_buffer.GetString(), json_buffer.Size());
    yx::crypt::xor_string(buf, json_buffer.Size(), secret_, sizeof(secret_));
    //
    p = packet;
  }
};
//////////////////////////////////////////////////////////////////////////
// SkynetServer，处理连接后端Skynet网络，用于网关发现，控制，监控等功能。
SkynetServer::SkynetServer()
  : stopping_(false)
  , socket_manager_(nullptr)
{

}

SkynetServer::~SkynetServer()
{

}


/*
@func			: Start
@brief		:
*/
void SkynetServer::Start(yx::SocketManager* socket_manager) {
  socket_manager_ = socket_manager;
  LOG(WARNING) << "SkynetServer::Start";
  //
  Config& cfg = Config::Instance();
  const char* master_host = cfg.getMasterHost();
  // 连接AP服务并转接到gateway manager服务中。
  static const char kLoginServer[] = "gateway";
  ConnectAP(master_host, kLoginServer);
}

/*
@func			: Stop
@brief		:
*/
void SkynetServer::Stop() {
  stopping_ = true;
  LOG(WARNING) << "SkynetServer::Stop";
}

/*
@func			: HandleConnect
@brief		:
*/
void SkynetServer::HandleConnect(uint64_t fd, yx::Packet& packet) {
  const uint8_t* buf = packet.buf();
  uint16_t buf_size = packet.buf_size();                                       
  int err = buf_size >= sizeof(int) ? *reinterpret_cast<const int*>(buf) : -1;
  //
  ap_map_t::iterator iter(ap_map_.find(fd));
  if (ap_map_.end() == iter) return;
  //
  if (0 == err) {
    yx::Packet connect_packet;
    iter->second->handle_connect(connect_packet);
    socket_manager_->Send(fd, connect_packet);
    LOG(WARNING) << "SkynetServer::Connecting ap_host:"<< iter->second->addr_ << " tcp_id:" << fd << " err:" << err;
  } else {
    LOG(WARNING) << "SkynetServer::Connecting ap_host:" << iter->second->addr_ << " tcp_id:" << fd << " err:" << err;
    ap_map_.erase(iter);
  }
}

/*
@func			: OnReconnect
@brief		:
*/
void SkynetServer::OnReconnect(const std::string& ap_host, const std::string& ap_server, yx::Timer* timer) {
  ConnectAP(ap_host.c_str(), ap_server.c_str());
  timer->Close();
}

/*
@func			: HandleClose
@brief		:
*/
void SkynetServer::HandleClose(uint64_t fd, yx::Packet& packet) {
  ap_map_t::iterator iter(ap_map_.find(fd));
  if (ap_map_.end() == iter) return;
  //
  AP* ap = iter->second.get();
  if (!stopping_) {
    yx::Timer* timer = yx::Timer::New(socket_manager_->loop());
    timer->set_cb(std::bind(&SkynetServer::OnReconnect, this, std::string(ap->addr_), std::string(ap->server_), std::placeholders::_1));
    timer->Start(3 * 60 * 1000);
  }
  //
  LOG(WARNING) << "skynet server ap:" << iter->second->server_ << " fd:" << fd << "disconnect";
  ap_map_.erase(iter);
}

/*
@func			: HandleData
@brief		:
*/
bool SkynetServer::HandleData(uint64_t tcp_id, yx::Packet& packet) {
  const uint8_t* buf = packet.buf();
  uint16_t buf_size = packet.buf_size();
  if (buf_size < sizeof(uint16_t) + sizeof(uint32_t)) return false;
  uint16_t cmd = 0;
  buf = yx::_read_u16(buf, &cmd);
  uint32_t session = 0, uid = 0;
  buf = yx::_read_u32(buf, &session);
  buf = yx::_read_u32(buf, &uid);
  buf_size = buf_size - (buf - packet.buf());
  //
  yx::Packet out_packet;
  auto ap_iter = ap_map_.find(tcp_id);
  if (ap_map_.end() == ap_iter) return false;
  AP* ap = ap_iter->second.get();
  //
  switch (cmd) {
  case static_cast<uint16_t>(NETCMD_S2C::CONNECT) :
    if (ap->handle_connect(out_packet)) {
      socket_manager_->Send(tcp_id, out_packet);
    }
    break;
  case static_cast<uint16_t>(NETCMD_S2C::CHALLENGE):
    if (ap->handle_challenge(buf, buf_size, out_packet))
      socket_manager_->Send(tcp_id, out_packet);
    break;
  case static_cast<uint16_t>(NETCMD_S2C::CONNECT_ACCEPT):
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ap->join_gateway_manager(out_packet);
    socket_manager_->Send(tcp_id, out_packet);   
    break;
  case static_cast<uint16_t>(NETCMD_S2C::CONNECT_REJECT):
    socket_manager_->Close(tcp_id);
    break;
  case static_cast<uint16_t>(NETCMD_S2C::JOIN_SERVER_RESPONSE):
    if (1 != uid) {
      LOG(WARNING) << "JOIN_SERVER FAILED";
      socket_manager_->Close(tcp_id);
    } else {
      // Ok
      LOG(WARNING) << "JOIN_SERVER OK";
#ifdef YX_GF
      this->ReqConnGateway(tcp_id);
#endif // YX_GF
    }
    break;
  case static_cast<uint16_t>(NETCMD_S2C::REQ_CONN_GATEWAY_RESPONSE): {
    yx::crypt::xor_string(const_cast<uint8_t*>(buf), buf_size, ap->secret_, sizeof(ap->secret_));
    //
    InputToBackend((TCP_OP::SKYNET_F2F), tcp_id, packet);
    }
    break;
  }
  return true;
  
}

/*
@func			: ConnectAP
@brief		: 连接Skynet AP服务。
*/
uint64_t SkynetServer::ConnectAP(const char ap_host[64], const char ap_server[64]) {
  LOG(WARNING) << "SkynetServer::Connect AP ap_host:" << ap_host << " ap_server:" << ap_server;
  std::unique_ptr<AP> ap(new SkynetServer::AP);
  strcpy(ap->server_, ap_server);
  strcpy(ap->addr_, ap_host);
  ap->state = 0;
  //
  uint64_t ap_id = socket_manager_->TcpConnect(ap_host, (TCP_OP::SKYNET_SERVER));
  if (ap_id > 0) {
    ap->tcp_id = ap_id;
    ap_map_.emplace(ap_map_t::value_type(ap_id, std::move(ap)));
  }
  //
  return ap_id;
}

/*
@func			: ReqLoginGateway
@brief		:
*/
bool SkynetServer::ReqConnGateway(uint64_t ap_id) {
  ap_map_t::iterator iter(ap_map_.find(ap_id));
  if (ap_map_.end() == iter) {
    return false;
  }
  AP* ap = iter->second.get();
  //
  Config& cfg = Config::Instance();
  const char* gateway_name = cfg.getGatewayName();
  uint16_t name_sz = strlen(gateway_name);
  //
  yx::Packet packet(2 + 4 + 4 + name_sz);
  // len, cmd, session, uid
  uint8_t* buf = yx::_write_u16(packet.mutable_buf(), static_cast<uint16_t>(NETCMD_C2S::REQ_CONN_GATEWAY));
  buf = yx::_write_u32(buf, 0);
  buf = yx::_write_u32(buf, 0);
  //
  memcpy(buf, gateway_name, name_sz);
  yx::crypt::xor_string(buf, name_sz, ap->secret_, sizeof(ap->secret_));
  //
  socket_manager_->Send(ap_id, packet);
  //
  return false;
}

// -------------------------------------------------------------------------
