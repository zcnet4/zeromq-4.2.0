/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\yw_server.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-14 14:02
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef AGENT_MANAGER_YW_H_
#define AGENT_MANAGER_YW_H_
#include <set>
#include <map>
#include <string>
#include <memory>
#include <unordered_map>
#include "yx/processor.h"
#include "agent_manager.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// 
struct pbc_env;
struct pbc_slice;
struct pbc_rmessage;
class Runner;
namespace yx {
  class Loop;
  class Packet;
}
//////////////////////////////////////////////////////////////////////////
// YWServer
class YWServer
  : private AgentManager
{
public:
  YWServer();
  ~YWServer();
public:
  using AgentManager::CreateAgent;
  using AgentManager::GetAgent;
  using AgentManager::CloseAgent;
  /*
  @func			: Start
  @brief		: 
  */
  bool Start(yx::Loop* loop);
  /*
  @func			: Stop
  @brief		: 
  */
  void Stop();
  /*
  @func			: processClientMsg
  @brief		: 
  */
  bool processClientMsg(Agent* agent, yx::Packet& packet);
  /*
  @func			: check_proto_c2s
  @brief		: 
  */
  bool check_proto_c2s(int proto_type);
  /*
  @func			: check_proto_s2c
  @brief		:
  */
  bool check_proto_s2c(int proto_type);
  /*
  @func			: findSpawnWorldId
  @brief		: 
  */
  uint32_t findSpawnWorldId();
  /*
  @func			: processGameMsg
  @brief		: 
  */
  bool processGameMsg(uint32_t world_id, uint32_t uid, uint32_t type, 
    const uint8_t* buf, uint16_t buf_size, yx::Packet& packet);
  /*
  @func			: processGameServerStatus
  @brief		: 
  */
  bool processGameServerStatus(const uint8_t* buf, uint16_t buf_size);
  /*
  @func			: removeAgent
  @brief		: 
  */
  void removeAgent(uint64_t vtcp_id);
  /*
  @func			: sendGatewayStartToGameServer
  @brief		: 
  */
  void sendGatewayStartToGameServer(uint32_t world_id);
  /*
  @func			: sendGatewayActiveToGameServer
  @brief		: 
  */
  void sendGatewayActiveToGameServer(uint32_t world_id);
private:
  /*
  @func			: init_protos
  @brief		: 
  */
  void init_protos();
  /*
  @func			: init_pbc_env
  @brief		: 
  */
  bool init_pbc_env();
  /*
  @func			: initSpawnWorldIdSet
  @brief		: 
  */
  bool initSpawnWorldIdSet();
  /*
  @func			: decodeClientMsg
  @brief		: 返回服务端命令，参数返回原始数据内容。
  */
  bool decodeClientMsg(uint32_t uid, yx::Packet& packet, uint16_t& msgType, pbc_slice* slice);
  /*
  @func			: ticks_now
  @brief		: 
  */
  uint64_t ticks_now() const;
  /*
  @func			: quitClientsByWorldId
  @brief		: 
  */
  void quitClientsByWorldId(uint32_t world_id);
private:
  class AgentYW;
  /*
  @func			: NewAgent
  @brief		:
  */
  virtual Agent* NewAgent();
  /*
  @func			: GetAgentFromUid
  @brief		: 
  */
  AgentYW* GetAgentFromUid(uint32_t uid);
  /*
  @func			: ProcessLogin
  @brief		:
  */
  bool processLogin(AgentYW* agent, pbc_slice* login_msg);
  /*
  @func			: parseLoginMsg
  @brief		:
  */
  bool parseLoginMsg(AgentYW* agent, pbc_rmessage* pmsg);
  /*
  @func			: processPendingLogin
  @brief		:
  */
  void processPendingLogin(AgentYW* agent, bool kick);
  /*
  @func			: askGameToLogout
  @brief		: 
  */
  void askGameToLogout(AgentYW* agent);
  /*
  @func			: askGameServerToAddPeer
  @brief		: 
  */
  void askGameServerToAddPeer(uint32_t world_id, uint32_t uid, pbc_slice* login_msg = nullptr);
  /*
  @func			: pushMsgToGameServer
  @brief		: 发到游戏服务器。
  */
  void pushMsgToGameServer(uint32_t world_id, int32_t uid, int32_t type, pbc_slice* data);
  /*
  @func			: sendMsgToClient
  @brief		: 发到窗户端。
  */
  bool sendMsgToClient(AgentYW* agent, int32_t type, pbc_slice* data);
  /*
  @func			: sendRawMsgToClient
  @brief		: 发到窗户端。
  */
  bool sendRawMsgToClient(AgentYW* agent, yx::Packet& packet);
  /*
  @func			: sendRawMsgToAllClient
  @brief		: 
  */
  void sendRawMsgToAllClient(uint32_t world_id, yx::Packet& packet);
  /*
  @func			: sendAccessDeniedToClient
  @brief		: 
  */
  void sendAccessDeniedToClient(AgentYW* client);
  /*
  @func			: onPlayerSaved
  @brief		: 
  */
  void onPlayerSaved(AgentYW* agent);
  void onPlayerSavedBeingKicked(AgentYW* agent);
  void onPlayerSavedLoggingOut(AgentYW* agent);
  void onPlayerSavedSwitchingWorld(AgentYW* agent);
  /*
  @func			: sendSwitchWorldAckToClient
  @brief		: 
  */
  void sendSwitchWorldAckToClient(AgentYW* agent, int result);
  /*
  @func			: buildQueuedMsg
  @brief		:
  */
  yx::Packet buildQueuedMsg(int32_t uid, int32_t type, pbc_slice* data);
  /*
  @func			: buildClientMsg
  @brief		:
  */
  yx::Packet buildClientMsg(int32_t type, pbc_slice* data);
private:
  yx::Loop* loop_;
  typedef std::map<int, std::string> protos_t;
  protos_t proto_c2s_;
  protos_t proto_s2c_;
  pbc_env* client_pbc_env_;
  pbc_env* server_pbc_env_;
  typedef std::set<uint32_t> spawn_world_ids_t;
  spawn_world_ids_t spawn_world_ids_;
  struct OnlineStatus {
    int num;
    uint64_t updateTime;
  };
  typedef std::map<uint32_t, OnlineStatus> game_server_status_t;
  game_server_status_t game_server_status_;
private:
  // 已登录列表。
  typedef std::map<uint32_t/*uid*/, uint64_t/*vtcp_id*/> logined_list_t;
  logined_list_t logined_list_;
  struct pending_login {
    int loging_msg_len;
    std::unique_ptr<char[]> loging_msg;
    uint64_t vtcp_id;
    pending_login(int len, uint64_t id)
      : loging_msg_len(len)
      , loging_msg(new char[len])
      , vtcp_id(id)
    {}
    pending_login(pending_login&& src) {
      loging_msg_len = src.loging_msg_len;
      loging_msg = std::move(src.loging_msg);
      vtcp_id = src.vtcp_id;
    }
  };
  typedef std::multimap<uint32_t/*uid*/, pending_login> pending_logins_t;
  pending_logins_t pending_logins_;
};


// -------------------------------------------------------------------------
#endif /* AGENT_MANAGER_YW_H_ */
