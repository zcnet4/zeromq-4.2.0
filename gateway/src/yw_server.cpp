/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\yw_server.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-14 14:02
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "yw_server.h"
#include "yx/yx_stl_util.h"
#include "yx/yx_util.h"
#include "yx/loop.h"
#include "yx/packet.h"
#include "pbc.h"
#include "protocol_defs.h"
#include "gateway_config.h"
#include "gateway_constants.h"
#include "frontend_processor.h"
#include "backend_processor.h"
#include "agent.h"
#include "yx/logging.h"
#include "op_defs.h"
#include "runner.h"
#include "gateway.h"
#include "yx/packet_view.h"
#include "yx/allocator.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// AgentYW
class YWServer::AgentYW
  : public ::Agent
  , public YXAllocObject<>
{
public:
  enum GW_CLIENT_STATUS {
    GW_CLIENT_STATUS_NORMAL,            // 正常状态
    GW_CLIENT_STATUS_SWITCHING_WORLD,   // 收到客户端switch world请求后进入该状态
    GW_CLIENT_STATUS_LOGGING_OUT,       // 收到客户端logout请求后进入该状态
    GW_CLIENT_STATUS_BEING_KICKED,      // 收到一个相同uid的INIT_LEGACY请求，老的client进入该状态
    GW_CLIENT_STATUS_DISCONNECT,        // yx增加断开状态。by ZC. 2016-11-29.
  };
public:
  AgentYW()
    : uid_(-1)
    , world_id_(-1)
    , to_world_id_(-1)
    , status_(GW_CLIENT_STATUS_NORMAL)
  {

  }
  ~AgentYW()
  {

  }
  class Guard{
  public:
    Guard(Agent* a, YWServer* server)
      : agent_(static_cast<YWServer::AgentYW*>(a))
      , server_(server)
    {
    }
    ~Guard() {
      if (YWServer::AgentYW::GW_CLIENT_STATUS_DISCONNECT == agent_->status()) {
        server_->CloseAgent(agent_->vtcp_id());
      }
    }
    YWServer::AgentYW* operator->() {
      return agent_;
    }
    operator YWServer::AgentYW*() {
      return agent_;
    }
  private:
    YWServer::AgentYW* agent_;
    YWServer* server_;
  };
public:
  uint32_t uid() const { return uid_; }
  void set_uid(uint32_t uid) { uid_ = uid; }
  uint32_t world_id() const { return world_id_; }
  void set_world_id(uint32_t world_id) { world_id_ = world_id; }
  uint32_t to_world_id() const { return to_world_id_; }
  void set_to_world_id(uint32_t to_world_id) { to_world_id_ = to_world_id; }
  GW_CLIENT_STATUS status() const { return status_; }
  void set_status(GW_CLIENT_STATUS status) { status_ = status; }
private:
  uint32_t uid_;
  uint32_t world_id_;
  uint32_t to_world_id_;
  GW_CLIENT_STATUS status_;
};

#ifdef YX_YW
pbc_env* g_yw_server_pbc_env = nullptr;
#endif
//////////////////////////////////////////////////////////////////////////
// YWServer
YWServer::YWServer()
  : loop_(nullptr)
  , client_pbc_env_(pbc_new())
  , server_pbc_env_(pbc_new())
{
#ifdef YX_YW
  // 为了少一次内存分配，为YW开个方便之门。by ZC。2016-12-9 17:44.
  g_yw_server_pbc_env = server_pbc_env_;
#endif
}

YWServer::~YWServer()
{

}

/*
@func			: Start
@brief		:
*/
bool YWServer::Start(yx::Loop* loop) {
  loop_ = loop;
  //
  init_protos();
  //
  if (!init_pbc_env()) {
    LOG(ERROR) << "YWServer::Initialize failed";
    return false;
  }
  //
  if (!initSpawnWorldIdSet()) {
    LOG(ERROR) << "YWServer::Initialize failed";
    return false;
  }
  //
  return true;
}

/*
@func			: init_protos
@brief		:
*/
void YWServer::init_protos() {
  // 协议号与协议名称映射表
  // C --> S
  proto_c2s_[TOSERVER_GAME_BREATH] = "TOSERVER_GAME_BREATH";
  proto_c2s_[TOSERVER_LOGIN] = "TOSERVER_LOGIN";
  proto_c2s_[TOSERVER_INIT] = "TOSERVER_INIT";
  proto_c2s_[TOSERVER_CLIENT_READY] = "TOSERVER_CLIENT_READY";
  proto_c2s_[TOSERVER_INTERACT] = "TOSERVER_INTERACT";
  proto_c2s_[TOSERVER_PLAYERPOS] = "TOSERVER_PLAYERPOS";
  proto_c2s_[TOSERVER_GOTBLOCKS] = "TOSERVER_GOTBLOCKS";
  proto_c2s_[TOSERVER_DELETEDBLOCKS] = "TOSERVER_DELETEDBLOCKS";
  proto_c2s_[TOSERVER_PLAYERITEM] = "TOSERVER_PLAYERITEM";
  proto_c2s_[TOSERVER_INVENTORY_ACTION] = "TOSERVER_INVENTORY_ACTION";
  proto_c2s_[TOSERVER_CHAT_MESSAGE] = "TOSERVER_CHAT_MESSAGE";
  proto_c2s_[TOSERVER_TIME_OF_DAY] = "TOSERVER_TIME_OF_DAY";
  proto_c2s_[TOSERVER_DAMAGE] = "TOSERVER_DAMAGE";
  proto_c2s_[TOSERVER_BREATH] = "TOSERVER_BREATH";
  proto_c2s_[TOSERVER_RESPAWN] = "TOSERVER_RESPAWN";
  proto_c2s_[TOSERVER_REMOVED_SOUNDS] = "TOSERVER_REMOVED_SOUNDS";
  proto_c2s_[TOSERVER_LOGOUT] = "TOSERVER_LOGOUT";
  proto_c2s_[TOSERVER_LANDGRAVE] = "TOSERVER_LANDGRAVE";
  proto_c2s_[TOSERVER_SHOP_ACTION] = "TOSERVER_SHOP_ACTION";
  proto_c2s_[TOSERVER_VILLAGE] = "TOSERVER_VILLAGE";
  proto_c2s_[TOSERVER_REMOVENODE] = "TOSERVER_REMOVENODE";
  proto_c2s_[TOSERVER_ACHIEVE_REQUIRE] = "TOSERVER_ACHIEVE_REQUIRE";
  proto_c2s_[TOSERVER_SWITCH_WORLD] = "TOSERVER_SWITCH_WORLD";
  proto_c2s_[TOSERVER_SIGNTEXT] = "TOSERVER_SIGNTEXT";
  proto_c2s_[TOSERVER_TIME_SYN] = "TOSERVER_TIME_SYN";
  proto_c2s_[TOSERVER_SKILL_CAST] = "TOSERVER_SKILL_CAST";
  proto_c2s_[TOSERVER_SKILL_SING_DONE] = "TOSERVER_SKILL_SING_DONE";
  proto_c2s_[TOSERVER_TITLE] = "TOSERVER_TITLE";
  proto_c2s_[TOSERVER_REMOVE_BUFF] = "TOSERVER_REMOVE_BUFF";
  proto_c2s_[TOSERVER_OFFLINE_MESSAGE] = "TOSERVER_OFFLINE_MESSAGE";
  proto_c2s_[TOSERVER_PLAYER_PITCH_YAW] = "TOSERVER_PLAYER_PITCH_YAW";
  proto_c2s_[TOSERVER_NPCTALK_OP] = "TOSERVER_NPCTALK_OP";
  proto_c2s_[TOSERVER_REVIVE] = "TOSERVER_REVIVE";
  proto_c2s_[TOSERVER_NEARCHAT] = "TOSERVER_NEARCHAT";
  proto_c2s_[TOSERVER_ANALYSE_BEHAVIOR] = "TOSERVER_ANALYSE_BEHAVIOR";
  proto_c2s_[TOSERVER_TRADE] = "TOSERVER_TRADE";


  // S --> C
  proto_s2c_[TOCLIENT_GAME_BREATH] = "TOCLIENT_GAME_BREATH";
  proto_s2c_[TOCLIENT_LOGIN] = "TOCLIENT_LOGIN";
  proto_s2c_[TOCLIENT_ADDNODE] = "TOCLIENT_ADDNODE";
  proto_s2c_[TOCLIENT_REMOVENODE] = "TOCLIENT_REMOVENODE";
  proto_s2c_[TOCLIENT_TIME_OF_DAY] = "TOCLIENT_TIME_OF_DAY";
  proto_s2c_[TOCLIENT_UPDATE_TIME_OF_DAY] = "TOCLIENT_UPDATE_TIME_OF_DAY";
  proto_s2c_[TOCLIENT_BLOCKDATA] = "TOCLIENT_BLOCKDATA";
  proto_s2c_[TOCLIENT_HP] = "TOCLIENT_HP";
  proto_s2c_[TOCLIENT_BREATH] = "TOCLIENT_BREATH";
  proto_s2c_[TOCLIENT_ACCESS_DENIED] = "TOCLIENT_ACCESS_DENIED";
  proto_s2c_[TOCLIENT_DEATHSCREEN] = "TOCLIENT_DEATHSCREEN";
  proto_s2c_[TOCLIENT_INVENTORY] = "TOCLIENT_INVENTORY";
  proto_s2c_[TOCLIENT_DETACHED_INVENTORY] = "TOCLIENT_DETACHED_INVENTORY";
  proto_s2c_[TOCLIENT_CHAT_MESSAGE] = "TOCLIENT_CHAT_MESSAGE";
  proto_s2c_[TOCLIENT_MOVE_PLAYER] = "TOCLIENT_MOVE_PLAYER";
  proto_s2c_[TOCLIENT_ACTIVE_OBJECT_REMOVE_ADD] = "TOCLIENT_ACTIVE_OBJECT_REMOVE_ADD";
  proto_s2c_[TOCLIENT_ACTIVE_OBJECT_MESSAGES] = "TOCLIENT_ACTIVE_OBJECT_MESSAGES";
  proto_s2c_[TOCLIENT_PLAY_SOUND] = "TOCLIENT_PLAY_SOUND";
  proto_s2c_[TOCLIENT_STOP_SOUND] = "TOCLIENT_STOP_SOUND";
  proto_s2c_[TOCLIENT_PRIVILEGES] = "TOCLIENT_PRIVILEGES";
  proto_s2c_[TOCLIENT_BLOCKDATA_BATCH] = "TOCLIENT_BLOCKDATA_BATCH";
  proto_s2c_[TOCLIENT_HUNGER] = "TOCLIENT_HUNGER";
  proto_s2c_[TOCLIENT_ON_ITEM_USE] = "TOCLIENT_ON_ITEM_USE";
  proto_s2c_[TOCLIENT_LANDGRAVE_OPERATION_RESULT] = "TOCLIENT_LANDGRAVE_OPERATION_RESULT";
  proto_s2c_[TOCLIENT_SHOP_ACTION_RESULT] = "TOCLIENT_SHOP_ACTION_RESULT";
  proto_s2c_[TOCLIENT_NPCTALK] = "TOCLIENT_NPCTALK";
  proto_s2c_[TOCLIENT_GAME_BREATH] = "TOCLIENT_GAME_BREATH";
  proto_s2c_[TOCLIENT_ACHIEVEACK] = "TOCLIENT_ACHIEVEACK";
  proto_s2c_[TOCLIENT_TIME_SYN] = "TOCLIENT_TIME_SYN";
  proto_s2c_[TOCLIENT_BUFF_ADD] = "TOCLIENT_BUFF_ADD";
  proto_s2c_[TOCLIENT_BUFF_END] = "TOCLIENT_BUFF_END";
  proto_s2c_[TOCLIENT_SKILL_CAST] = "TOCLIENT_SKILL_CAST";
  proto_s2c_[TOCLIENT_SKILL_OVER] = "TOCLIENT_SKILL_OVER";
  proto_s2c_[TOCLIENT_SKILL_SING_CANCEL] = "TOCLIENT_SKILL_SING_CANCEL";
  proto_s2c_[TOCLIENT_SKILL_SING_OVER] = "TOCLIENT_SKILL_SING_OVER";
  proto_s2c_[TOCLIENT_TITLES_LOAD] = "TOCLIENT_TITLES_LOAD";
  proto_s2c_[TOCLIENT_SKILL_UPDATE] = "TOCLIENT_SKILL_UPDATE";
  proto_s2c_[TOCLIENT_CURRENT_TITLE] = "TOCLIENT_CURRENT_TITLE";
  proto_s2c_[TOCLIENT_PLAYER_EXP] = "TOCLIENT_PLAYER_EXP";
  proto_s2c_[TOCLIENT_ENCHANT_TABLE] = "TOCLIENT_ENCHANT_TABLE";
  proto_c2s_[TOCLIENT_PLAYER_INFO] = "TOCLIENT_PLAYER_INFO";
  proto_s2c_[TOCLIENT_OPEN_MAP] = "TOCLIENT_OPEN_MAP";
  proto_c2s_[TOCLIENT_NEARCHAT] = "TOCLIENT_NEARCHAT";
  proto_s2c_[TOCLIENT_SHOW_TIPS] = "TOCLIENT_SHOW_TIPS";
}

/*
@func			: init_pbc_env
@brief		:
*/
bool YWServer::init_pbc_env() {
  int result = -1;
  Config& cfg = Config::Instance();
  std::string pb_client(cfg.GetRealPath(cfg.getPBClient()));
  std::vector<uint8_t> client_buf;
  if (yx::util::ReadFileToBuffer(pb_client.c_str(), &client_buf)) {
    pbc_slice slice;
    slice.buffer = (char*)vector_as_array(&client_buf);
    slice.len = client_buf.size();
    result = pbc_register(client_pbc_env_, &slice);
  }
  if (0 != result) {
    LOG(ERROR) << "Cannot load proto_client.pb!!!";
    return false;
  }
  result = -1;
  std::string pb_server(cfg.GetRealPath(cfg.getPBServer()));
  std::vector<uint8_t> server_buf;
  if (yx::util::ReadFileToBuffer(pb_server.c_str(), &server_buf)) {
    pbc_slice slice;
    slice.buffer = (char*)vector_as_array(&server_buf);
    slice.len = server_buf.size();
    result = pbc_register(server_pbc_env_, &slice);
  }
  if (0 != result) {
    LOG(ERROR) << "Cannot load proto_server.pb!!!";
    return false;
  }
  return true;
}

/*
@func			: initSpawnWorldIdSet
@brief		:
*/
bool YWServer::initSpawnWorldIdSet() {
  std::vector<std::string> idStrVector = Config::Instance().getSpawnWorldIds();
  size_t size = idStrVector.size();
  LOG(INFO) << "spawn world id num:" << size;
  for (size_t i = 0; i < size; ++i) {
    int worldId = atoi(idStrVector[i].c_str());
    LOG(INFO) << "spawn world id:" << worldId;

    spawn_world_ids_.insert(worldId);
  }

  if (spawn_world_ids_.size() <= 0) {
    LOG(ERROR) << "spawn_world_ids config error, check it!";
    //exit(-1);
  }
  //
  return spawn_world_ids_.size() != 0;
}

/*
@func			: Stop
@brief		:
*/
void YWServer::Stop() {
  // 通知GameServer，所有端都退出。
  auto cb = [this](Agent* agent) {
    AgentYW* a = static_cast<AgentYW*>(agent);
    askGameToLogout(a);
    a->set_status(AgentYW::GW_CLIENT_STATUS_LOGGING_OUT);
  };
  for_each(cb);
}

/*
@func			: check_proto_c2s
@brief		:
*/
bool YWServer::check_proto_c2s(int proto_type) {
  return proto_c2s_.end() != proto_c2s_.find(proto_type);
}

/*
@func			: check_proto_s2c
@brief		:
*/
bool YWServer::check_proto_s2c(int proto_type) {
  return proto_s2c_.end() != proto_s2c_.find(proto_type);
}

/*
@func			: ticks_now
@brief		:
*/
uint64_t YWServer::ticks_now() const {
  return loop_->ticks_now();
}

/*
@func			: NewAgent
@brief		:
*/
Agent* YWServer::NewAgent() {
  return new YWServer::AgentYW();
}

/*
@func			: removeAgent
@brief		:
*/
void YWServer::removeAgent(uint64_t vtcp_id) {

}

/*
@func			: findSpawnWorldId
@brief		:
*/
uint32_t YWServer::findSpawnWorldId() {
  int spawnWorldId = -1;
  int minOnlineNum = 99999;
  for (auto it = spawn_world_ids_.begin(); it != spawn_world_ids_.end(); ++it) {
    int worldId = *it;
    auto mapIter = game_server_status_.find(worldId);
    if (mapIter != game_server_status_.end()) {
      if ((ticks_now() - mapIter->second.updateTime < 60 || spawnWorldId == -1)
        && mapIter->second.num < minOnlineNum) {
        spawnWorldId = worldId;
        minOnlineNum = mapIter->second.num;
      }
    }
  }

  if (spawnWorldId == -1) {
    std::stringstream ss;
    ss << "findSpawnWorldId failed!!, m_spawnWorldIdSet.size:" << spawn_world_ids_.size()
      << ",m_gameServerStatus.size:" << game_server_status_.size() << ",g_timeNow="
      << loop_->ticks_now() << "; ";

    for (auto it = spawn_world_ids_.begin(); it != spawn_world_ids_.end(); ++it) {
      ss << "spawn world id:" << *it << ",";
    }

    for (auto mapIter = game_server_status_.begin(); mapIter != game_server_status_.end(); ++mapIter) {
      ss << "game server status, id=" << mapIter->first << ",num=" << mapIter->second.num
        << ",updateTime=" << mapIter->second.updateTime << "; ";
    }

    LOG(ERROR) << ss.str();
  }

  return spawnWorldId;
}

/*
@func			: pushMsgToGameServer
@brief		: 发到游戏服务器。
*/
void YWServer::pushMsgToGameServer(uint32_t world_id, int32_t uid, int32_t type, pbc_slice* data) {
  yx::Packet packet(buildQueuedMsg(uid, type, data));
  //
  InputToBackend((TCP_OP::YW_M2B), world_id, packet);
}

/*
@func			: sendMsgToClient
@brief		: 发到窗户端。
*/
bool YWServer::sendMsgToClient(AgentYW* agent, int32_t type, pbc_slice* data) {
  yx::Packet packet(buildClientMsg(type, data));
  packet.set_param(0);
  //
  return sendMsgToClientImpl(agent, packet);
}

/*
@func			: sendMsgToClient
@brief		: 发到窗户端。
*/
bool YWServer::sendMsgToClientImpl(AgentYW* agent, yx::Packet& packet) {
  if (nullptr == agent) {
    LOG(WARNING) << "xxxx";
    return false;
  }
  yx::PacketView view(packet);
  view.inset(packet.offset());
  uint8_t* buf = view.buf();
  // encode
  for (int i = 0, count = view.buf_size(); i < count; i++) {
    buf[i] ^= 165;
  }
  //
  uint16_t op = true ? (TCP_OP::YW_M2F) : (TCP_OP::YW_G2GF);
  InputToFrontend(op, agent->vtcp_id(), packet);
  //
  return true;
}

/*
@func			: processGameServerStatus
@brief		:
*/
bool YWServer::processGameServerStatus(const uint8_t* buf, uint16_t buf_size) {
  pbc_slice slice;
  slice.buffer = (void*)buf;
  slice.len = buf_size;
  pbc_rmessage* s2s_online_num = pbc_rmessage_new(server_pbc_env_, kPROTO_S2SOnlineNum, &slice);
  if (s2s_online_num) {
    OnlineStatus onlineStatus;
    uint32_t worldId = pbc_rmessage_integer(s2s_online_num, kPROTO_S2SOnlineNumWorldId, 0, nullptr);
    onlineStatus.num = pbc_rmessage_integer(s2s_online_num, kPROTO_S2SOnlineNumOnlineNum, 0, nullptr);
    onlineStatus.updateTime = ticks_now();

    game_server_status_[worldId] = onlineStatus;

    //LOG(DEBUG) << "recordOnlineNum, worldId:" << worldId << ",num:" << onlineStatus.num
    //  << ",updateTime:" << onlineStatus.updateTime;

    //
    pbc_rmessage_delete(s2s_online_num);
    return true;
  } else {
    LOG(ERROR) << "parse s2s_online_num from game server failed";
    return false;
  }  
}

/*
@func			: processGameMsg
@brief		:
*/
bool YWServer::processGameMsg(uint32_t uid, uint32_t type, const uint8_t* buf, uint16_t buf_size, yx::Packet& packet) {
  bool success = false;
  switch (type) {
  case QMT_INVALID:
    LOG(ERROR) << "Gateway recv invalid msg from game server. ignore it.";
    break;
  case QMT_REPORT_ONLINE_NUM:
    success = processGameServerStatus(buf, buf_size);
    break;
  case QMT_DEL_PEER:
  {
    AgentYW* agent = GetAgentFromUid(uid);
    if (agent) {
      if (agent->status() == AgentYW::GW_CLIENT_STATUS_NORMAL) {
        CloseAgent(agent->vtcp_id());
      }
    }
    else {
      LOG(WARNING) << "can not find fd of uid " << uid << ", remove client fail";
    }
  }
    //if (fd != -1) {
    //  GWRemoteClient* client = clientMananger->getClientByFd(fd);
    //  if (NULL != client) {
    //    if (client->m_Status == GW_CLIENT_STATUS_NORMAL) {
    //      clientMananger->clearLogoutTime(client->m_Uid);
    //      clientMananger->removeClient(fd);
    //    }
    //  }
    //}
    //else {
    //}
    break;
  case QMT_PEER_LOGTOU_AND_SAVED: {
    LOG(INFO) << "recv onPlayerLogoutAndSaved of user, uid=" << uid;
    AgentYW* agent = GetAgentFromUid(uid);
    if (agent)
      onPlayerSaved(agent);
  }
    //clientMananger->clearLogoutTime(qm->uid());
    //onPlayerSaved(qm->uid());
    break;
  case QMT_GAME:
  {
    packet.set_param(buf - packet.buf());
    sendMsgToClientImpl(GetAgentFromUid(uid), packet);
  }
    break;
  default:
    LOG(ERROR) << "Gateway recv msg from game server, unknown type:" << type;
    break;
  }
  //
  return success;
}

/*
@func			: onPlayerSaved
@brief		:
*/
void YWServer::onPlayerSaved(AgentYW* agent) {

  LOG(INFO) << "client status is " << agent->status() << ",uid="
    << agent->uid() << ",fd=" << agent->vtcp_id();

  switch (agent->status()) {
  case AgentYW::GW_CLIENT_STATUS_SWITCHING_WORLD:
    onPlayerSavedSwitchingWorld(agent);
    return;
  case AgentYW::GW_CLIENT_STATUS_BEING_KICKED:
    onPlayerSavedBeingKicked(agent);
    return;
  case AgentYW::GW_CLIENT_STATUS_LOGGING_OUT:
    onPlayerSavedLoggingOut(agent);
    return;
  default:
    return;
  }
}

void YWServer::onPlayerSavedBeingKicked(AgentYW* agent) {
  processPendingLogin(agent, true);
  CloseAgent(agent->vtcp_id());
}

void YWServer::onPlayerSavedLoggingOut(AgentYW* agent) {
  processPendingLogin(agent, false);
  CloseAgent(agent->vtcp_id());
}

void YWServer::onPlayerSavedSwitchingWorld(AgentYW* agent) {
  LOG(INFO) << "user " << agent->uid() << " switch world succ, from " << agent->world_id()
    << " to " << agent->to_world_id();
  //
  askGameServerToAddPeer(agent->to_world_id(), agent->uid(), nullptr);
  sendSwitchWorldAckToClient(agent, 0);

  agent->set_world_id(agent->to_world_id());
  agent->set_to_world_id(-1);
  agent->set_status(AgentYW::GW_CLIENT_STATUS_NORMAL);
}

/*
@func			: sendSwitchWorldAckToClient
@brief		:
*/
void YWServer::sendSwitchWorldAckToClient(AgentYW* agent, int result) {
  // send s2c_switch_world to client
  pbc_wmessage* wmsg = pbc_wmessage_new(server_pbc_env_, kPROTO_S2CSwitchWorld);
  if (wmsg) {
    pbc_wmessage_integer(wmsg, kPROTO_S2CSwitchWorldResult, result, 0);
    pbc_wmessage_integer(wmsg, kPROTO_S2CSwitchWorldToWorldId, agent->to_world_id(), 0);
    //
    pbc_slice serialize_slice;
    pbc_wmessage_buffer(wmsg, &serialize_slice);
    sendMsgToClient(agent, TOCLIENT_SWITCH_WORLD_ACK, &serialize_slice);
    //
    pbc_wmessage_delete(wmsg);
  }
}

/*
@func			: buildQueuedMsg
@brief		:
*/
yx::Packet YWServer::buildQueuedMsg(int32_t uid, int32_t type, pbc_slice* data) {
  pbc_wmessage* queued_msg = pbc_wmessage_new(server_pbc_env_, kPROTO_QueuedMsg);
  if (queued_msg) {
    pbc_wmessage_integer(queued_msg, kPROTO_QueuedMsgUid, uid, 0);
    pbc_wmessage_integer(queued_msg, kPROTO_QueuedMsgType, type, 0);
    if (data)
      pbc_wmessage_string(queued_msg, kPROTO_QueuedMsgData, (const char*)data->buffer, data->len);
    //
    pbc_slice serialize_slice;
    pbc_wmessage_buffer(queued_msg, &serialize_slice);
    //
    yx::Packet packet(serialize_slice.len);
    memcpy(packet.mutable_buf(), serialize_slice.buffer, serialize_slice.len);
    pbc_wmessage_delete(queued_msg);
    //
    return packet;
  }
  //
  return yx::Packet();
}

/*
@func			: decode
@brief		: 
*/
pbc_rmessage* YWServer::decodeClientMsg(yx::Packet& packet, pbc_slice* slice) {
  uint8_t* pp = const_cast<uint8_t*>(packet.buf() + packet.offset());
  int pp_size = packet.buf_size() - packet.offset();
  for (int i = 0; i < pp_size; i++) {
    pp[i] ^= 165;
  }
  //
  slice->buffer = (void*)pp;
  slice->len = pp_size;
  //
  return pbc_rmessage_new(client_pbc_env_, kPROTO_Message, slice);
}

/*
@func			: buildClientMsg
@brief		:
*/
yx::Packet YWServer::buildClientMsg(int32_t type, pbc_slice* data) {
  pbc_wmessage* pmsg = pbc_wmessage_new(client_pbc_env_, kPROTO_Message);
  if (pmsg) {
    pbc_wmessage_integer(pmsg, kPROTO_MessageType, type, 0);
    if (data)
      pbc_wmessage_string(pmsg, kPROTO_MessageMsgData, (const char*)data->buffer, data->len);
    //
    pbc_slice serialize_slice;
    pbc_wmessage_buffer(pmsg, &serialize_slice);
    //
    yx::Packet packet(serialize_slice.len);
    memcpy(packet.mutable_buf(), serialize_slice.buffer, serialize_slice.len);
    pbc_wmessage_delete(pmsg);
    return packet;
  }
  //
  return yx::Packet();
}

/*
@func			: processClientMsg
@brief		:
*/
bool YWServer::processClientMsg(Agent* a, yx::Packet& packet) {
  AgentYW::Guard agent(a, this);
  {
    switch (agent->status()) {
    case AgentYW::GW_CLIENT_STATUS_SWITCHING_WORLD:
      LOG(INFO) << "client is switching world, drop msg";
      return false;
    case AgentYW::GW_CLIENT_STATUS_LOGGING_OUT:
      /**
      * 当client处于正在退出态时（即已经给game server发送了LOGOUT消息）
      * 所有消息都丢弃，包括SWITCH_WORLD消息。
      * 特别注意，如果SWITCH_WORLD消息没有被丢弃导致client状态转换为SWITCHING_WORLD
      * 这样将导致收到game server时忽略删除client，导致僵尸client一直存在。
      */
      LOG(INFO) << "client is logging out, drop msg";
      return false;
    case AgentYW::GW_CLIENT_STATUS_BEING_KICKED:
      LOG(INFO) << "client of uid:" << agent->uid() << " fd:" << agent->vtcp_id()
        << " is being kicked, ignore msg from the client";
      return false;
    default:
      break;
    }
  }
  // 解析消息。
  pbc_slice slice;
  pbc_rmessage* pmsg = decodeClientMsg(packet, &slice);
  if (nullptr == pmsg) {
    LOG(ERROR) << "Gateway parse client msg failed, drop the msg and continue";
    askGameToLogout(agent);
    return false;
  }
  // 协议类型
  uint32_t proto_type = pbc_rmessage_integer(pmsg, kPROTO_MessageType, 0, 0);
  if (!check_proto_c2s(proto_type)) {
    LOG(ERROR) << "Gateway recv client msg, but not supported type: " << proto_type << " check it now.";
    pbc_rmessage_delete(pmsg);
    return false;
  }
  //　是否已登录。
  if (!agent->auth()) {
    if (parseLoginMsg(agent, pmsg)) {
      processLogin(agent, &slice);
    } else {
      // delete agent
      agent->set_status(AgentYW::GW_CLIENT_STATUS_DISCONNECT);
    }
  } else {
    // 转发到后端处理器中。
    pushMsgToGameServer(agent->world_id(), agent->uid(), QMT_GAME, &slice);
  }
  pbc_rmessage_delete(pmsg);
  //
  return true;
}

/*
@func			: parseLoginMsg
@brief		:
*/
bool YWServer::parseLoginMsg(AgentYW* agent, pbc_rmessage* pmsg) {
  bool success = false;
  pbc_slice slice;
  slice.buffer = (void*)pbc_rmessage_string(pmsg, kPROTO_MessageMsgData, 0, &slice.len);
  //
  pbc_rmessage* rmsg = pbc_rmessage_new(client_pbc_env_, kPROTO_C2SLogin, &slice);
  if (rmsg) {
    agent->set_uid(pbc_rmessage_integer(rmsg, kPROTO_C2SLoginUid, 0, nullptr));
    agent->set_world_id(pbc_rmessage_integer(rmsg, kPROTO_C2SLoginWorldId, 0, nullptr));
    if (0 == agent->world_id()) {
      agent->set_world_id(findSpawnWorldId());
    }
    LOG(INFO) << "getUidWorldIdFromLoginMsg, uid:" << agent->uid() << ",worldId:" << agent->world_id();
    if (-1 == agent->world_id()) {
      LOG(ERROR) << "can not get uid and worldId, begin to close the connection";
    } else {
      success = true;
    }
  } else {
    LOG(ERROR) << "Gateway parse c2s_login fail, check it now!!!";
  }
  //
  pbc_rmessage_delete(rmsg);
  //
  return success;
}

/*
@func			: ProcessLogin
@brief		:
*/
bool YWServer::processLogin(AgentYW* agent, pbc_slice* login_msg) {
  uint32_t uid = agent->uid();
  logined_list_t::iterator iter(logined_list_.find(uid));
  if (logined_list_.end() == iter) {
    logined_list_.emplace(logined_list_t::value_type(uid, agent->vtcp_id()));
    agent->set_auth(true);
    askGameServerToAddPeer(agent->world_id(), uid, login_msg);
    return true;
  }
  // 处理重复登录的用户。
  AgentYW* old_user = (AgentYW*)GetAgent(iter->second);
  if (!old_user) {
    logined_list_[uid] = agent->vtcp_id();
    agent->set_auth(true);
    askGameServerToAddPeer(agent->world_id(), uid, login_msg);
    return true;
  }

  LOG(INFO) << "same uid client login, uid:" << uid
    << ", old fd:" << old_user->vtcp_id() << ", new fd:" << agent->vtcp_id()
    << ", original new client fd:" << 0
    << ", old client status:" << old_user->status();
  //
  if (old_user->status() == AgentYW::GW_CLIENT_STATUS_BEING_KICKED) {
    /*
    * 已经有一个新的客户端想踢掉这个客户端，并且还未完成
    */
    auto range = pending_logins_.equal_range(uid);
    for (; range.second != range.first; ++ range.first) {
      pending_login& pl = range.first->second;
      AgentYW* a = static_cast<AgentYW*>(GetAgent(pl.vtcp_id));
      if (a && a->status() == AgentYW::GW_CLIENT_STATUS_NORMAL) {
        a->set_status(AgentYW::GW_CLIENT_STATUS_BEING_KICKED);
        sendAccessDeniedToClient(a);
      }
    } // for
    pending_login pl(login_msg->len, agent->vtcp_id());
    memcpy(pl.loging_msg.get(), login_msg->buffer, login_msg->len);
    pending_logins_.emplace(uid, std::move(pl));
    return true;

  } else if (old_user->status() == AgentYW::GW_CLIENT_STATUS_LOGGING_OUT) {
    auto range = pending_logins_.equal_range(uid);
    for (; range.second != range.first; ++range.first) {
      pending_login& pl = range.first->second;
      AgentYW* a = static_cast<AgentYW*>(GetAgent(pl.vtcp_id));
      if (a && a->status() == AgentYW::GW_CLIENT_STATUS_NORMAL) {
        a->set_status(AgentYW::GW_CLIENT_STATUS_BEING_KICKED);
        sendAccessDeniedToClient(a);
      }
    } // for
    pending_login pl(login_msg->len, agent->vtcp_id());
    memcpy(pl.loging_msg.get(), login_msg->buffer, login_msg->len);
    pending_logins_.emplace(uid, std::move(pl));
    return true;
  } else {
    // old client 属于正常状态
    sendAccessDeniedToClient(old_user);
    askGameToLogout(old_user);
    old_user->set_status(AgentYW::GW_CLIENT_STATUS_BEING_KICKED);
    pending_login pl(login_msg->len, agent->vtcp_id());
    memcpy(pl.loging_msg.get(), login_msg->buffer, login_msg->len);
    pending_logins_.emplace(uid, std::move(pl));
    return true;
  }
  return true;
}

/*
@func			: processPendingLogin
@brief		:
*/
void YWServer::processPendingLogin(AgentYW* agent, bool kick) {
  uint32_t uid = agent->uid();
  // removeClient, 删除已登录列表。
  auto iter(logined_list_.find(uid));
  if (logined_list_.end() != iter)
    logined_list_.erase(iter);
  //
  pending_login* pending = nullptr; {
    // 只取出最后一个做登录。
    auto range = pending_logins_.equal_range(uid);
    for (; range.second != range.first; ++range.first) {
      pending = &(range.first->second);
    }
  }
  AgentYW* agent_new = pending ? static_cast<AgentYW*>(GetAgent(pending->vtcp_id)) : nullptr;
  if (agent_new) {
    if (kick) {
      LOG(INFO) << "client of uid: " << uid << ",fd:" << agent->vtcp_id() << " kicked,"
        << "new fd:" << agent_new->vtcp_id();
    } else {
      LOG(INFO) << "client of uid: " << uid << ",fd:" << agent->vtcp_id() << " logging out,"
        << "new fd:" << agent_new->vtcp_id();
    }
    pbc_slice login_msg;
    login_msg.buffer = pending->loging_msg.get();
    login_msg.len = pending->loging_msg_len;
    processLogin(agent_new, &login_msg);
  }
  else {
    if (kick) {
      LOG(ERROR) << "kicking client, new client of new fd is NULL";
    }
  }
  //
  pending_logins_.erase(uid);
}

/*
@func			: askGameServerToAddPeer
@brief		: 
*/
void YWServer::askGameServerToAddPeer(uint32_t world_id, uint32_t uid, pbc_slice* login_msg/* = nullptr*/) {
  pushMsgToGameServer(world_id, uid, QMT_ADD_PEER, nullptr);
  LOG(INFO) << "send add peer msg to game server,uid:" << uid;
  if (login_msg) {
    // 并将登录消息转发到Game Server中。
    pushMsgToGameServer(world_id, uid, QMT_GAME, login_msg);
  }
}

/*
@func			: askGameToLogout
@brief		:
*/
void YWServer::askGameToLogout(AgentYW* agent) {
  if (agent->status() == AgentYW::GW_CLIENT_STATUS_BEING_KICKED) {
    // 如果该客户端正在被踢当中，则不需再次发生logout给game server
    return;
  }

  if (agent->status() == AgentYW::GW_CLIENT_STATUS_LOGGING_OUT) {
    // 如果该客户端已经发送了logout，则不需再次发生logout给game server
    return;
  }
  //
  pbc_wmessage* msg = pbc_wmessage_new(client_pbc_env_, kPROTO_Message);
  if (msg) {
    pbc_wmessage_integer(msg, kPROTO_MessageType, TOSERVER_LOGOUT, 0);
    pbc_wmessage* c2s_logout = pbc_wmessage_new(client_pbc_env_, kPROTO_C2SLogout);
    if (c2s_logout) {
      pbc_wmessage_integer(c2s_logout, kPROTO_C2SLogoutParam, 7777, 0);
      //
      pbc_slice slice;
      pbc_wmessage_buffer(c2s_logout, &slice);
      pbc_wmessage_string(msg, kPROTO_MessageMsgData, (const char*)slice.buffer, slice.len);
      //
      pbc_wmessage_delete(c2s_logout);
    }
    pbc_slice slice;
    pbc_wmessage_buffer(msg, &slice);
    pushMsgToGameServer(agent->world_id(), agent->uid(), QMT_GAME, &slice);
    pbc_wmessage_delete(msg);
  }
  /*c2s_logout logout;
  logout.set_param(7777);

  Message *msg = new Message();
  msg->set_type(TOSERVER_LOGOUT);
  msg->set_msg_data(logout.SerializeAsString());

  QueuedMsg* qmsg = new QueuedMsg();
  qmsg->set_uid(uid);
  qmsg->set_type(QMT_GAME);
  qmsg->set_data(msg->SerializeAsString());
  delete msg;

  m_RecvMsgsQueue.push_back(qmsg);

  updateLogoutTime(uid);
  LOG(TRACE) << "askGameToLogout, uid:" << uid;*/
}

/*
@func			: sendAccessDeniedToClient
@brief		:
*/
void YWServer::sendAccessDeniedToClient(YWServer::AgentYW* client) {
  pbc_wmessage* wmsg = pbc_wmessage_new(server_pbc_env_, kPROTO_S2CAccessDenied);
  if (wmsg) {
    pbc_wmessage_integer(wmsg, kPROTO_S2CAccessDeniedCode, SERVER_ACCESSDENIED_CLIENT_EXISTS, 0);
    pbc_slice serialize_slice;
    pbc_wmessage_buffer(wmsg, &serialize_slice);
    //
    sendMsgToClient(client, TOCLIENT_ACCESS_DENIED, &serialize_slice);
    //
    pbc_wmessage_delete(wmsg);
  }
  LOG(INFO) << "sendAccessDeniedToClient, uid:" << client->uid() << ",fd:" << client->vtcp_id();
}


/*
@func			: GetAgentFromUid
@brief		:
*/
YWServer::AgentYW* YWServer::GetAgentFromUid(uint32_t uid) {
  logined_list_t::iterator iter(logined_list_.find(uid));
  if (logined_list_.end() != iter) {
    uint64_t& vtcp_id = iter->second;
    return static_cast<AgentYW*>(GetAgent(vtcp_id));
  }
  //
  LOG(ERROR) << "can not find client by uid " << uid;
  return nullptr;
}

// -------------------------------------------------------------------------
