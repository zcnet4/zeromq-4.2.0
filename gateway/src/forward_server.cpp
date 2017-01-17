/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\forward_server.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-28 11:24
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "forward_server.h"
#include "gateway_util.h"
#include "op_defs.h"
#include "gateway_config.h"
#include "yx/socket_manager.h"
#include "yx/packet.h"
#include "op_defs.h"
#include "yx/logging.h"
#include "gateway_util.h"
#include "netproto.h"
#include "gateway.h"
#include <algorithm>
// -------------------------------------------------------------------------
ForwardServer::ForwardServer()
  : socket_manager_(nullptr)
{

}


ForwardServer::~ForwardServer()
{

}

/*
@func			: Start
@brief		:
*/
void ForwardServer::Start(yx::SocketManager* socket_manager) {
  socket_manager_ = socket_manager;
  LOG(WARNING) << "ForwardServer::Start";
}

/*
@func			: Stop
@brief		:
*/
void ForwardServer::Stop() {
  for (auto iter(channels_.begin()), iterEnd(channels_.end()); iterEnd != iter; ++iter) {
    Channel& c = *iter;
    socket_manager_->Close(c.tcp_id);
  }
  //
  channels_.clear();
  LOG(WARNING) << "ForwardServer::Stop";
}

/*
@func			: ConnectGateway
@brief		: gateway-forward连接gateway建议信道。
*/
void ForwardServer::ConnectGateway(const char* gateway_host) {
  Channel c;
  c.tcp_id = socket_manager_->TcpConnect(gateway_host, (TCP_OP::FORWARD_SERVER));
  c.connected = 0;
  c.session = 0;
  c.mss = 0;
  channels_.push_back(c);
}


/*
@func			: HandleConnect
@brief		:
*/
void ForwardServer::HandleConnect(uint64_t tcp_id, yx::Packet& packet) {
  auto iter = std::find_if(channels_.begin(), channels_.end(), [tcp_id](channels_t::value_type& v) {
    return tcp_id == v.tcp_id ? true : false;
  });
  if (channels_.end() == iter) {
    LOG(ERROR) << "ForwardServer::HandleConnect can not handle:" << tcp_id;
    return;
  }
  //
  int err = *reinterpret_cast<const int*>(packet.buf());
  if (0 == err) {
    Channel& c = *iter;
    c.ids.clear();
    c.connected = 1;
    c.mss = 0;
    c.session = 0;
  } else {
    channels_.erase(iter);
    LOG(ERROR) << "ForwardServer::HandleConnect err code:" << err;
  }
}

/*
@func			: HandleClose
@brief		:
*/
void ForwardServer::HandleClose(uint64_t tcp_id, yx::Packet& packet) {
  LOG(WARNING) << "ForwardServer::HandleClose tcp_id:" << tcp_id;
  auto iter = std::find_if(channels_.begin(), channels_.end(), [tcp_id](channels_t::value_type& v) {
    return tcp_id == v.tcp_id ? true : false;
  });
  if (channels_.end() != iter) {
    ids_t& ids = iter->ids;
    // 断开连接将清除所有client cache。
    for (auto it(ids.begin()), itEnd(ids.end()); itEnd != it; ++it) {
      cache_.erase(*it);
    }
    channels_.erase(iter);
  }
}

/*
@func			: HandleData
@brief		:
*/
void ForwardServer::HandleData(uint64_t tcp_id, yx::Packet& packet) {
  //[len][cmd][session][uid][content]:包长度+命令+会话+uid+内容。
  const uint8_t* buf = packet.buf();
  uint16_t cmd = 0, offset = 0;;
  uint32_t session = 0, uid = 0;
  buf = yx::_read_u16(buf, &cmd);
  buf = yx::_read_u32(buf, &session);
  buf = yx::_read_u32(buf, &uid);
  offset = buf - packet.buf();
  //
  if (cmd != static_cast<uint16_t>(NETCMD_G2G::G2GF_FORWARD)) {
    return;
  }
  {
    uint16_t cmd = 0;
    buf = yx::_read_u16(buf, &cmd);
    uint32_t session = 0, uid = 0;
    buf = yx::_read_u32(buf, &session);
    buf = yx::_read_u32(buf, &uid);
    LOG(WARNING) << "g recv session:" << session;
  }
  uint64_t client_id = uid;
  client_id <<= 32;
  client_id |= session & 0x00000000FFFFFFFF;
  packet.set_param(offset);
  InputToFrontend((TCP_OP::FORWARD_B2F), client_id, packet);
}


/*
@func			: Forward
@brief		:
*/
void ForwardServer::ForwardImpl(uint64_t client_id, const yx::Packet& packet) {
  Channel* c = GetTcpChannel(client_id);
  if (0 == c->tcp_id) return;
  //int N = packet.buf_size() / c->mss;
  //yx::Packet header(sizeof(uint16_t) + sizeof(uint32_t) + sizeof())
}

/*
@func			: Forward
@brief		: 转发到
*/
void ForwardServer::Forward(uint64_t client_id, uint32_t client_session, const yx::Packet& packet) {
  Channel* c = GetTcpChannel(client_id);
  if (!c || !c->connected) return;
  //[len][cmd][session][uid][content]:包长度+命令+会话+uid+内容。
  yx::Packet header(sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + packet.buf_size());
  uint8_t* buf = header.mutable_buf();
  // len, cmd, session, uid
  buf = yx::_write_u16(buf, static_cast<uint16_t>(NETCMD_G2G::GF2G_FORWARD));
  buf = yx::_write_u32(buf, client_id & 0x00000000FFFFFFFF);
  buf = yx::_write_u32(buf, (client_id >> 32) & 0x00000000FFFFFFFF);
  //
  memcpy(buf, packet.buf(), packet.buf_size());
  //tcp_server_->Send2(tcp_id, header, packet);
  socket_manager_->Send(c->tcp_id, header);
  {
    const uint8_t* buf = packet.buf();
    uint16_t cmd = 0;
    buf = yx::_read_u16(buf, &cmd);
    uint32_t session = 0, uid = 0;
    buf = yx::_read_u32(buf, &session);
    buf = yx::_read_u32(buf, &uid);
    LOG(WARNING) << "forward session:" << session;
  }
}

/*
@func			: ForwardClose
@brief		:
*/
void ForwardServer::ForwardClose(uint64_t client_id, const yx::Packet& packet) {
  channel_cache_t::iterator iter(cache_.find(client_id));
  if (cache_.end() != iter) {
    Channel* c = iter->second;
    for (auto it(c->ids.begin()), itEnd(c->ids.end()); it != itEnd; ++it) {
      if (client_id == *it) {
        c->ids.erase(it);
        break;
      }
    } // for
    uint64_t tcp_id = c->tcp_id;
    cache_.erase(iter);
    // 向网关发送Close命令。
    //[len][cmd][session][uid][content]:包长度+命令+会话+uid+内容。
    yx::Packet header(sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t));
    uint8_t* buf = header.mutable_buf();
    // len, cmd, session, uid
    buf = yx::_write_u16(buf, static_cast<uint16_t>(NETCMD_G2G::GF2G_FORWARD_CLOSE));
    buf = yx::_write_u32(buf, client_id & 0x0000FFFF);
    buf = yx::_write_u32(buf, (client_id >> 32) & 0x0000FFF);
    //
    socket_manager_->Send(tcp_id, header);
  }
}

/*
@func			: GetTcpChannel
@brief		:
*/
ForwardServer::Channel* ForwardServer::GetTcpChannel(uint64_t client_id) {
  channel_cache_t::iterator iter(cache_.find(client_id));
  Channel* c = nullptr;
  if (cache_.end() != iter) {
    c = iter->second;
  } else {
    size_t min_ids_size = (size_t)-1;
    for (auto it(channels_.begin()), itEnd(channels_.end()); it != itEnd; ++it) {
      size_t ids_size = it->ids.size();
      if (ids_size < min_ids_size) {
        min_ids_size = ids_size;
        c = it.operator ->();
      }
    } // for
    if (nullptr == c) {
      LOG(ERROR) << "ForwardServer::GetTcpId No connection is available!!!";
      return nullptr;
    } else {
      cache_.emplace(channel_cache_t::value_type(client_id, c));
      c->ids.push_back(client_id);
    }
  }
  //
  return c;
}

// -------------------------------------------------------------------------
