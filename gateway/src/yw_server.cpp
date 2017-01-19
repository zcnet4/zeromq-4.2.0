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
#include "yx/packet_view.h"
#include "pbc.h"
#include "networkprotocol.h"
#include "gateway_gameserver_def.h"
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
#include <algorithm>
#include <cstdlib>
#ifdef OS_WIN
#define rand_r(x) rand()
#endif // OS_WIN
// -------------------------------------------------------------------------
#define PROTO_HEAD_SIZE 4
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
    GW_CLIENT_STATUS_LOGGING_OUT,        // 收到客户端logout请求后进入该状态
    GW_CLIENT_STATUS_IN_QUEUE,          // yx增加在排队状态。by ZC. 2017-1-19.
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
  uint64_t uid() const { return uid_; }
  void set_uid(uint64_t uid) { uid_ = uid; }
  uint32_t world_id() const { return world_id_; }
  void set_world_id(uint32_t world_id) { world_id_ = world_id; }
  uint32_t to_world_id() const { return to_world_id_; }
  void set_to_world_id(uint32_t to_world_id) { to_world_id_ = to_world_id; }
  GW_CLIENT_STATUS status() const { return status_; }
  void set_status(GW_CLIENT_STATUS status) { status_ = status; }
private:
  uint64_t uid_;
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
  /*if (!initSpawnWorldIdSet()) {
    LOG(ERROR) << "YWServer::Initialize failed";
    return false;
  }*/
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
  proto_c2s_[TOSERVER_LOGOUT] = "TOSERVER_LOGOUT";
  proto_c2s_[TOSERVER_LANDGRAVE] = "TOSERVER_LANDGRAVE";
  proto_c2s_[TOSERVER_SHOP_ACTION] = "TOSERVER_SHOP_ACTION";
  proto_c2s_[TOSERVER_VILLAGE] = "TOSERVER_VILLAGE";
  proto_c2s_[TOSERVER_REMOVENODE] = "TOSERVER_REMOVENODE";
  proto_c2s_[TOSERVER_ACHIEVE_REQUIRE] = "TOSERVER_ACHIEVE_REQUIRE";
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
  proto_c2s_[TOSERVER_GET_FURNACE_STATUS] = "TOSERVER_GET_FURNACE_STATUS";
  proto_c2s_[TOSERVER_QUEUE_SERVER_VALIDATE] = "TOSERVER_QUEUE_SERVER_VALIDATE";
  proto_c2s_[TOSERVER_TELEPORT] = "TOSERVER_TELEPORT";
  proto_c2s_[TOSERVER_UPDATE_NODE_STYLE] = "TOSERVER_UPDATE_NODE_STYLE";
  proto_c2s_[TOSERVER_MAPNODE_INVENTORY] = "TOSERVER_MAPNODE_INVENTORY";
  proto_c2s_[TOSERVER_BOOKTABOPEN_STATUS] = "TOSERVER_BOOKTABOPEN_STATUS";
  proto_c2s_[TOSERVER_GET_ACHIEVEREWARD] = "TOSERVER_GET_ACHIEVEREWARD";
  proto_c2s_[TOSERVER_SWITCH_WORLD_CHECK] = "TOSERVER_SWITCH_WORLD_CHECK";
  // S --> C
  proto_s2c_[TOCLIENT_GAME_BREATH] = "TOCLIENT_GAME_BREATH";
  proto_s2c_[TOCLIENT_SERVER_READY] = "TOCLIENT_SERVER_READY";
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
  proto_s2c_[TOCLIENT_FURNACE_STATUS_ACK] = "TOCLIENT_FURNACE_STATUS_ACK";
  proto_s2c_[TOCLIENT_CAN_SEND_LOGIN] = "TOCLIENT_QUEUE_SERVER_PASS";
  proto_s2c_[TOCLIENT_TELEPORT] = "TOCLIENT_TELEPORT";
  proto_s2c_[TOCLIENT_MAPNODE_INVENTORY] = "TOCLIENT_MAPNODE_INVENTORY";
  proto_s2c_[TOSERVER_BOOKTABOPEN_STATUS] = "TOSERVER_BOOKTABOPEN_STATUS";
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
//bool YWServer::initSpawnWorldIdSet() {
//  std::vector<std::string> idStrVector = Config::Instance().getSpawnWorldIds();
//  size_t size = idStrVector.size();
//  LOG(INFO) << "spawn world id num:" << size;
//  for (size_t i = 0; i < size; ++i) {
//    int worldId = atoi(idStrVector[i].c_str());
//    LOG(INFO) << "spawn world id:" << worldId;
//
//    spawn_world_ids_.insert(worldId);
//  }
//
//  if (spawn_world_ids_.size() <= 0) {
//    LOG(ERROR) << "spawn_world_ids config error, check it!";
//    //exit(-1);
//  }
//  //
//  return spawn_world_ids_.size() != 0;
//}

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
@func			: time_now
@brief		:
*/
uint64_t YWServer::time_now() const {
  return loop_->time_now();
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
//uint32_t YWServer::findSpawnWorldId() {
//  int spawnWorldId = -1;
//  int minOnlineNum = 99999;
//  for (auto it = spawn_world_ids_.begin(); it != spawn_world_ids_.end(); ++it) {
//    int worldId = *it;
//    auto mapIter = game_server_status_.find(worldId);
//    if (mapIter != game_server_status_.end()) {
//      if ((ticks_now() - mapIter->second.updateTime < 60 || spawnWorldId == -1)
//        && mapIter->second.num < minOnlineNum) {
//        spawnWorldId = worldId;
//        minOnlineNum = mapIter->second.num;
//      }
//    }
//  }
//
//  if (spawnWorldId == -1) {
//    std::stringstream ss;
//    ss << "findSpawnWorldId failed!!, m_spawnWorldIdSet.size:" << spawn_world_ids_.size()
//      << ",m_gameServerStatus.size:" << game_server_status_.size() << ",g_timeNow="
//      << loop_->ticks_now() << "; ";
//
//    for (auto it = spawn_world_ids_.begin(); it != spawn_world_ids_.end(); ++it) {
//      ss << "spawn world id:" << *it << ",";
//    }
//
//    for (auto mapIter = game_server_status_.begin(); mapIter != game_server_status_.end(); ++mapIter) {
//      ss << "game server status, id=" << mapIter->first << ",num=" << mapIter->second.num
//        << ",updateTime=" << mapIter->second.updateTime << "; ";
//    }
//
//    LOG(ERROR) << ss.str();
//  }
//
//  return spawnWorldId;
//}

/*
@func			: pushMsgToGameServer
@brief		: 发到游戏服务器。
*/
bool YWServer::pushMsgToGameServer(uint32_t world_id, int64_t uid, int32_t type, pbc_slice* data) {
  auto iter(games_.find(world_id));
  if (games_.end() == iter || !iter->second.m_GameServerActive) {
    return false;
  }
  yx::Packet packet(buildQueuedMsg(uid, type, data));
  InputToBackend((TCP_OP::YW_M2B), world_id, packet);
  //
  return true;
}

/*
@func			: sendMsgToClient
@brief		: 发到窗户端。
*/
bool YWServer::sendMsgToClient(AgentYW* agent, int32_t type, pbc_slice* data) {
  yx::Packet packet(buildClientMsg(type, data));
  //
  return sendRawMsgToClient(agent, packet);
}

/*
@func			: sendRawMsgToClient
@brief		: 发到窗户端。
*/
bool YWServer::sendRawMsgToClient(AgentYW* agent, yx::Packet& packet) {
  if (nullptr == agent) {
    LOG(WARNING) << "xxxx";
    return false;
  }
  /*这里不做Xor，GS已做了Xor。by ZC. 2017-1-18 20:13.
  yx::PacketView view(packet);
  view.inset(packet.offset());
  uint8_t* buf = view.buf();
  // encode
  for (int i = 0, count = view.buf_size(); i < count; i++) {
    buf[i] ^= 165;
  }
  */
  //
  uint16_t op = true ? (TCP_OP::YW_M2F) : (TCP_OP::YW_G2GF);
  InputToFrontend(op, agent->vtcp_id(), packet);
  //
  return true;
}

/*
@func			: sendRawMsgToAllClient
@brief		:
*/
void YWServer::sendRawMsgToAllClient(uint32_t world_id, yx::Packet& packet) {
  for_each([this, &packet, &world_id](Agent* a) {
    AgentYW* agent = static_cast<AgentYW*>(a);
    if (agent->world_id() == world_id)
      sendRawMsgToClient(agent, packet);
  });
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
    onlineStatus.updateTime = time_now();

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
@func			: processQueueServerValidateResult
@brief		:
*/
void YWServer::processQueueServerValidateResult(uint32_t world_id, uint64_t uid, int pass) {
  auto iter(pending_logins2_.find(uid));
  if (pending_logins2_.end() != iter) {
    pending_login& l = iter->second;
    if (l.login_count > 1) {
      // 在验证过程中，不止一个客户端发送了排队服务器验证协议，这里只处理最后一个
      l.login_count--;
      return;
    }
    uint64_t vtcp_id = l.vtcp_id;
    pending_logins2_.erase(iter);
    if (pass == 0) {  // 通知新的客户端排队服务器验证失败
      //DenyAccess(newClient, SERVER_ACCESSDENIED_QUEUE_SERVER_VALIDATE_FAILED);
    } else {
      auto it(uid_logineds_.find(uid));
      if (uid_logineds_.end() != it) {
        //sendAccessDeniedToClient(oldClient);
        //askGameToLogout(oldClient->m_CurrWorldId, oldClient->m_Uid, oldClient->m_Status);
        uid_logineds_.erase(it);
      }
      login_t login(vtcp_id);
      uid_logineds_.emplace(uid_logineds_t::value_type(uid, login));
      sendToClientCanSendLogin(world_id, uid, vtcp_id);
    }
  } else {
    // 正常客户端的排队验证
    auto it(uid_logineds_.find(uid));
    if (uid_logineds_.end() != it) {
      if (pass == 0) { // 通知客户端排队服务器验证失败
      //    DenyAccess(client, SERVER_ACCESSDENIED_QUEUE_SERVER_VALIDATE_FAILED);
      } else {  // 排队服务器验证通过，通知客户端可以发送登录协议
        sendToClientCanSendLogin(world_id, uid, it->second.vtcp_id);
      }
    }
  }
}


/*
@func			: processGameMsg
@brief		:
*/
bool YWServer::processGameMsg(uint32_t world_id, uint64_t uid, uint32_t type,
  const uint8_t* buf, uint16_t buf_size, yx::Packet& packet) {
  bool success = false;
  switch (type) {
  case QMT_INVALID:
    LOG(ERROR) << "Gateway recv invalid msg from game server. ignore it.";
    break;
  case QMT_REPORT_ONLINE_NUM:
    success = processGameServerStatus(buf, buf_size);
    break;
  case QMT_DEL_PEER: {
    AgentYW* agent = GetAgentFromUid(uid);
    if (agent) {
      if (agent->status() == AgentYW::GW_CLIENT_STATUS_NORMAL) {
        CloseAgent(agent->vtcp_id());
      }
    }
    else {
      LOG(WARNING) << "can not find fd of uid " << uid << ", remove client fail";
    }
  } break;
  /*case QMT_PEER_LOGTOU_AND_SAVED: {
    LOG(INFO) << "recv onPlayerLogoutAndSaved of user, uid=" << uid;
    AgentYW* agent = GetAgentFromUid(uid);
    if (agent)
      onPlayerSaved(agent);
  }*/
    //clientMananger->clearLogoutTime(qm->uid());
    //onPlayerSaved(qm->uid());
    break;
  case QMT_GAME: {
    // 配置好偏移量。
    packet.set_param(buf - packet.buf() + sizeof(uint16_t));
    // 这里不做Xor，GS已做了Xor。by ZC. 2017-1-18 20:13.
    sendRawMsgToClient(GetAgentFromUid(uid), packet);
  } break;
  case QMT_GAME_BROADCAST: {
    // 配置好偏移量。
    packet.set_param(buf - packet.buf() + sizeof(uint16_t));
    // 这里不做Xor，GS已做了Xor。by ZC. 2017-1-18 20:13.
    sendRawMsgToAllClient(world_id, packet);
  } break;
  case QMT_QUEUESERVER_RESULT: {
    char string_buf[128] = { 0 };
    memcpy(string_buf, buf, std::min<uint16_t>(buf_size, 128));
    processQueueServerValidateResult(world_id, uid, atoi(string_buf));
  } break;
  case QMT_GAME_BREATHE: {
    auto iter(games_.find(world_id));
    if (games_.end() != iter) {
      Game& game = iter->second;
      game.m_GameServerActive = true;
      game.m_LastBreatheTimestamp = time_now();
    } else {
      Game game;
      game.m_GameServerActive = true;
      game.m_LastBreatheTimestamp = time_now();
      games_.emplace(games_t::value_type(world_id, game));
    }
    LOG(INFO) << "Gateway recv game breathe.";
  } break;
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
  case AgentYW::GW_CLIENT_STATUS_LOGGING_OUT:
    onPlayerSavedLoggingOut(agent);
    return;
  default:
    return;
  }
}

void YWServer::onPlayerSavedBeingKicked(AgentYW* agent) {
  CloseAgent(agent->vtcp_id());
}

void YWServer::onPlayerSavedLoggingOut(AgentYW* agent) {
  CloseAgent(agent->vtcp_id());
}

void YWServer::onPlayerSavedSwitchingWorld(AgentYW* agent) {
  LOG(INFO) << "user " << agent->uid() << " switch world succ, from " << agent->world_id()
    << " to " << agent->to_world_id();
  //
  //askGameServerToAddPeer(agent->to_world_id(), agent->uid(), nullptr);
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
yx::Packet YWServer::buildQueuedMsg(int64_t uid, int32_t type, pbc_slice* data) {
  pbc_wmessage* queued_msg = pbc_wmessage_new(server_pbc_env_, kPROTO_QueuedMsg);
  if (queued_msg) {
    pbc_wmessage_integer(queued_msg, kPROTO_QueuedMsgUid, static_cast<int32_t>(uid), 0);
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
@func			: decodeClientMsg
@brief		: 返回服务端命令，参数返回原始数据内容。
*/
bool YWServer::decodeClientMsg(uint64_t uid, yx::Packet& packet, uint16_t& msgType, pbc_slice* slice) {
  //因为YW需要把包长度两个字节发到GameServer中，而yx框架会默认去掉包长度两个字节。
  //故这里做适配。
  CHECK(0 == packet.offset());
  uint8_t* pp = const_cast<uint8_t*>(packet.buf() - sizeof(uint16_t));
  int pp_size = packet.buf_size() + sizeof(uint16_t);
  yx::_write_u16(pp, pp_size);
//  通信协议头
/*
 0       7        15       23      31
 --------|--------|--------|--|------
       mLen       |   mCmd    | mFlag
*/
  if (pp_size < PROTO_HEAD_SIZE) {
    // should not come here
    LOG(ERROR) << "client change protocol, uid:" << uid;
    //askGameToLogout(client->m_CurrWorldId, client->m_Uid, client->m_Status);
    return false;
  }
  uint8_t v0 = *(pp + 2);
  uint8_t v1 = *(pp + 3);
  msgType = ((v1 >> 6) << 8) | v0;
  if ((v1 & 0x3F) != 0x02) {
    return false;
  }
  slice->buffer = (void*)pp;
  slice->len = pp_size;
  //
  return true;
}

/*
@func			: buildClientMsg
@brief		:
*/
yx::Packet YWServer::buildClientMsg(int32_t type, pbc_slice* data) {
 /*
 0       7        15       23      31
 --------|--------|--------|--|------
       mLen       |   mCmd    | mFlag
 */
  yx::Packet packet(sizeof(uint16_t) + data->len);
  packet.set_param(0);
  uint8_t* buf = packet.mutable_buf();
  uint16_t buf_size = packet.buf_size();
  //
  buf[0] = type & 0xff;
  buf[1] = 0;
  buf[1] |= ((type >> 8) << 6);
  uint8_t encryptType = 1;
  buf[1] |= (encryptType << 1);
  buf += sizeof(uint16_t);
  buf_size -= sizeof(uint16_t);
  // Xor
  CHECK(buf_size == data->len);
  memcpy(buf, data->buffer, buf_size);
  for (int i = 0; i < buf_size; i++) {
    buf[i] ^= 165;
  }
  //
  return packet;
}

/*
@func			: processClientMsg
@brief		:
*/
bool YWServer::processClientMsg(Agent* a, yx::Packet& packet) {
  AgentYW::Guard agent(a, this);
  // 解析消息。
  uint16_t proto_type = 0;
  pbc_slice slice;
  if (!decodeClientMsg(agent->uid(), packet, proto_type, &slice)) {
    askGameToLogout(agent);
    return false;
  }
  if (!check_proto_c2s(proto_type)) {
    LOG(ERROR) << "check it, Gateway recv client msg, but not supported type: " << proto_type;
    askGameToLogout(agent);
    return false;
  }  
  //　是否已登录。
  if (!agent->auth()) {
    // 处理排队服务器协议
    if (proto_type == TOSERVER_QUEUE_SERVER_VALIDATE) {
      if (processQueueServerMsg(agent, &slice)) {
        agent->set_status(AgentYW::GW_CLIENT_STATUS_IN_QUEUE);
        return true;
      } else {
        // delete agent
        agent->set_status(AgentYW::GW_CLIENT_STATUS_DISCONNECT);
        return false;
      }
    } else {
      if (AgentYW::GW_CLIENT_STATUS_IN_QUEUE != agent->status()) {
        // Client连接成功后，发送过来的第一条协议不是登录协议，踢掉Client
        LOG(ERROR) << "client first protocol is not LOGIN, remove it :" << proto_type;
        askGameToLogout(agent);
        return false;
      }
    }
    // 处理登录协议
    if (proto_type == TOSERVER_LOGIN) {
      bool login_success = processClientLoginMsg(agent, &slice);
      if (login_success) {
        agent->set_auth(true);
        return true;
      } else {
        // delete agent
        agent->set_status(AgentYW::GW_CLIENT_STATUS_DISCONNECT);
        return false;
      }
    }
    
  } else {
    if (agent->status() != AgentYW::GW_CLIENT_STATUS_NORMAL) {
      LOG(INFO) << "client is switching world, drop msg";
      return false;
    }
    // 转发到后端处理器中。
    pushMsgToGameServer(agent->world_id(), agent->uid(), QMT_GAME, &slice);
  }
  //
  return true;
}

/*
@func			: askGameToLogout
@brief		:
*/
void YWServer::askGameToLogout(AgentYW* agent) {
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
@func			: sendToClientCanSendLogin
@brief		:
*/
void YWServer::sendToClientCanSendLogin(uint32_t world_id, uint64_t uid, uint64_t vtcp_id) {
  auto iter(uid_logineds_.find(uid));
  if (uid_logineds_.end() == iter) return;
  //
  login_t& login = iter->second;
  uint64_t now = time_now();
  login.m_LoginSessionCreateTime = now;
  login.world_id = world_id;
  login.m_LoginSession = reinterpret_cast<uint64_t>(&login) + rand_r(&now);
  login.swicthWorldSession = reinterpret_cast<uint64_t>(&login) + rand_r(&now);
  //
  pbc_wmessage* wmsg = pbc_wmessage_new(server_pbc_env_, kPROTO_S2CClientCanSendLogin);
  if (wmsg) {
    pbc_wmessage_integer(wmsg, kPROTO_S2CClientCanSendLoginWorldId, login.world_id, 0);
    pbc_wmessage_integer(wmsg, kPROTO_S2CClientCanSendLoginLoginSession, login.m_LoginSession & 0x0FFFFFFFF, login.m_LoginSession >> 32);
    pbc_wmessage_integer(wmsg, kPROTO_S2CClientCanSendLoginSwitchWorldSession, login.m_LoginSession & 0x0FFFFFFFF, login.m_LoginSession >> 32);
    //
    pbc_slice serialize_slice;
    pbc_wmessage_buffer(wmsg, &serialize_slice);
    sendMsgToClient(static_cast<AgentYW*>(GetAgent(login.vtcp_id)), TOCLIENT_CAN_SEND_LOGIN, &serialize_slice);
    //
    pbc_wmessage_delete(wmsg);
  }
}


/*
@func			: GetAgentFromUid
@brief		:
*/
YWServer::AgentYW* YWServer::GetAgentFromUid(uint64_t uid) {
  uid_logineds_t::iterator iter(uid_logineds_.find(uid));
  if (uid_logineds_.end() != iter) {
    uid_logineds_t::mapped_type& l = iter->second;
    return static_cast<AgentYW*>(GetAgent(l.vtcp_id));
  }
  //
  LOG(ERROR) << "can not find client by uid " << uid;
  return nullptr;
}

/*
@func			: sendGatewayStartToGameServer
@brief		:
*/
void YWServer::sendGatewayStartToGameServer(uint32_t world_id) {
  Game game;
  games_.emplace(games_t::value_type(world_id, game));
  //
  yx::Packet packet(buildQueuedMsg(0, QMT_GATEWAY_START, nullptr));
  InputToBackend((TCP_OP::YW_M2B), world_id, packet);
}

/*
@func			: sendGatewayActiveToGameServer
@brief		:
*/
void YWServer::sendGatewayActiveToGameServer(uint32_t world_id) {
  // 先关闭之前登录到world_id的客户端。
  quitClientsByWorldId(world_id);
  //
  yx::Packet packet(buildQueuedMsg(0, QMT_GATEWAY_ACTIVE, nullptr));
  InputToBackend((TCP_OP::YW_M2B), world_id, packet);
}

/*
@func			: quitClientsByWorldId
@brief		:
*/
void YWServer::quitClientsByWorldId(uint32_t world_id) {
  std::vector<uint64_t> ids;
  for_each([&ids, world_id](Agent* a){
    AgentYW* agent = static_cast<AgentYW*>(a);
    if (world_id == agent->world_id()){
      ids.push_back(agent->vtcp_id());
    }
  });
  for (auto iter(ids.begin()), iterEnd(ids.end()); iterEnd != iter; ++iter) {
    CloseAgent(*iter);
  }
}

/*
@func			: processQueueServerMsg
@brief		:
*/
bool YWServer::processQueueServerMsg(YWServer::AgentYW* agent, pbc_slice* msg) {
  //
  {
    // 这里默认解密是XOR，没压缩。
    uint8_t xor_buffer[64 * 1026] = { 0 };
    pbc_slice slice;
    slice.buffer = xor_buffer;
    slice.len = msg->len - PROTO_HEAD_SIZE;
    uint8_t* proto_data = static_cast<uint8_t*>(msg->buffer) + PROTO_HEAD_SIZE;
    for (int i = 0; i < slice.len; i++) {
      xor_buffer[i] = proto_data[i] ^ 165;
    }
    pbc_rmessage* rmsg = pbc_rmessage_new(client_pbc_env_, kPROTO_C2SQueueServerValidate, &slice);
    if (rmsg) {
      uint32_t uid_h = 0;
      uint32_t uid_l = pbc_rmessage_integer(rmsg, kPROTO_C2SQueueServerValidateId, 0, &uid_h);
      //uint64_t uid = (uint64_t)uid_h << 32 | uid_l;
      agent->set_uid((uint64_t)uid_h << 32 | uid_l);
      agent->set_world_id(pbc_rmessage_integer(rmsg, kPROTO_C2SQueueServerValidateWorldId, 0, nullptr));
      pbc_rmessage_delete(rmsg);
    } else {
      return false;
    }
  }
  //
  auto iter(games_.find(agent->world_id()));
  if (games_.end() == iter) {
    // 客户端尝试进入不存在的世界，直接踢掉客户端
    return false;
  }
  Game& g = iter->second;
  if (!g.m_GameServerActive) {
    // 客户端进入不可用的世界，发送协议告诉客户端应该下线
    //DenyAccess(client, SERVER_ACCESSDENIED_GAMESERVER_INACTIVE);
    return false;
  }
  //
  AgentYW* old_client = GetAgentFromUid(agent->uid());
  if (!old_client) {
    /**
    * 玩家第一次发送登录协议包, 继续正常登录流程
    */
    /**
    * 找不到之前登录的client, 依然继续正常登录流程
    */
    login_t login(agent->vtcp_id());
    uid_logineds_.emplace(uid_logineds_t::value_type(agent->uid(), login));
    LOG(INFO) << "user first login, send init legacy msg to game, uid:" << agent->uid()
      << ", fd:" << agent->vtcp_id();
    pushMsgToGameServer(agent->world_id(), agent->uid(), QMT_CLIENT, msg);
    return true;
  }
  // 已经有相同账号的客户端在线，准备把老的踢下线
  if (old_client->world_id() != agent->world_id()) {
    // 新的排队服务器验证协议，尝试在其他世界进行验证，非法操作
    return false;
  }
  /**
  * 执行到这里，表明当前已经有一个该uid对应的连接，
  * 该连接可能是客户端断开了还未来得及清理，或者玩家多个手机端登录
  */
  LOG(INFO) << "same uid client login, uid:" << agent->uid()
    << ", old fd:" << old_client->vtcp_id() << ", new fd:" << agent->vtcp_id()
    //<< ", original new client fd:" << oldClient->m_NewFd
    << ", old client status:" << old_client->status();

  pending_login l;
  /* 如果老的客户端处m_NewFd为－1，踢掉其他尝试登录的客户端*/
  auto it(pending_logins2_.find(agent->uid()));
  if (pending_logins2_.end() != it) {
    //  GWRemoteClient* originalNewClient = getClientByFd(oldClient->m_NewFd);
    //  if (originalNewClient != NULL) {  // 踢掉其他尝试登录的客户端
    //    originalNewClient->m_Status = GW_CLIENT_STATUS_LOGGING_OUT;
    //    sendAccessDeniedToClient(originalNewClient);
    //  }
    l = it->second;
  } else {
    l.login_count = 0;
  }
  // 在验证过程中，不止一个客户端发送了排队服务器验证协议，这里只处理最后一个
  l.login_count++;
  l.vtcp_id = agent->vtcp_id();
  pending_logins2_[agent->uid()] = l;
  if (!pushMsgToGameServer(agent->world_id(), agent->uid(), QMT_CLIENT, msg)) {
    // 世界id不存在，或者该世界当前处于不可用状态（心跳超时），直接丢弃消息，这时不应该主动断开连接
    LOG(ERROR) << "drop msg for worldId=" << agent->world_id() << ",uid=" << agent->uid();
  }
  return true;
}

/*
@func			: processClientLoginMsg
@brief		:
*/
bool YWServer::processClientLoginMsg(AgentYW* agent, pbc_slice* msg) {
  uint32_t world_id = 0;
  uint64_t login_session = 0;
  {
    uint8_t xor_buffer[64 * 1026] = { 0 };
    pbc_slice slice;
    slice.buffer = xor_buffer;
    slice.len = msg->len - PROTO_HEAD_SIZE;
    uint8_t* proto_data = static_cast<uint8_t*>(msg->buffer) + PROTO_HEAD_SIZE;
    for (int i = 0; i < slice.len; i++) {
      xor_buffer[i] = proto_data[i] ^ 165;
    }
    pbc_rmessage* rmsg = pbc_rmessage_new(client_pbc_env_, kPROTO_C2SLogin, &slice);
    if (rmsg) {
      uint32_t login_session_h = 0;
      uint32_t login_session_l = pbc_rmessage_integer(rmsg, kPROTO_C2SLoginLoginSession, 0, &login_session_h);
      login_session = (uint64_t)login_session_h << 32 | login_session_l;
      world_id = pbc_rmessage_integer(rmsg, kPROTO_C2SLoginWorldId, 0, nullptr);
      pbc_rmessage_delete(rmsg);
    } else {
      return false;
    }
  }
  //
  auto iter = uid_logineds_.find(agent->uid());
  if (uid_logineds_.end() == iter) {
    LOG(ERROR) << "gateway can not find LoginInfo, uid= " << agent->uid();
    return false;
  }
  login_t& login = iter->second;
  if (login.m_LoginSession != login_session) {
    // (1) session校验失败，踢掉客户端
    LOG(ERROR) << "login session error, uid:" << agent->uid();
    //toRemoveClient = true;
    return false;
  } else if ((time_now() - login.m_LoginSessionCreateTime) > 10 * 1000/*10s*/) {
    // (2) session超时，踢掉客户端
    LOG(ERROR) << "login session expired, uid:" << agent->uid();
    return false;
  } else if (login.world_id != world_id) {
    // (3) 客户端要进入的世界与网关允许进入的世界id不一致，踢掉客户端
    LOG(ERROR) << "login worldid error, gateway worldid:" << login.world_id
      << ", login worldid:" << world_id << ", uid:" << agent->uid();
    return false;
  } else {
    login.m_LoginSession = 0;
    login.m_LoginSessionCreateTime = 0;
    pushMsgToGameServer(agent->world_id(), agent->uid(), QMT_CLIENT, msg);
    return true;  // 登录协议校验通过，可以向gameserver转发登录协议
  }
  //
  return false;
}


// -------------------------------------------------------------------------
