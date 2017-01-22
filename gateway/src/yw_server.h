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
  @func			: processGameMsg
  @brief		: 
  */
  bool processGameMsg(uint32_t world_id, uint64_t uid, uint32_t type, 
    const uint8_t* buf, uint16_t buf_size, yx::Packet& packet);
  /*
  @func			: processGameServerStatus
  @brief		: 
  */
  bool processGameServerStatus(const uint8_t* buf, uint16_t buf_size);
  /*
  @func			: processQueueServerValidateResult
  @brief		: 
  */
  void processQueueServerValidateResult(uint32_t world_id, uint64_t uid, int pass);
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
  //bool initSpawnWorldIdSet();
  /*
  @func			: findSpawnWorldId
  @brief		:
  */
  //uint32_t findSpawnWorldId();
  /*
  @func			: decodeClientMsg
  @brief		: 返回服务端命令，参数返回原始数据内容。
  */
  bool decodeClientMsg(uint64_t uid, yx::Packet& packet, uint16_t& msgType, pbc_slice* slice);
  /*
  @func			: time_now
  @brief		: 
  */
  uint64_t time_now() const;
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
  AgentYW* GetAgentFromUid(uint64_t uid);
  /*
  @func			: processQueueServerMsg
  @brief		:
  */
  bool processQueueServerMsg(AgentYW* agent, pbc_slice* msg);
  /*
  @func			: processClientLoginMsg
  @brief		: 
  */
  bool processClientLoginMsg(AgentYW* agent, pbc_slice* msg);
  /*
  @func			: askGameToLogout
  @brief		: 
  */
  void askGameToLogout(AgentYW* agent);
  /*
  @func			: pushMsgToGameServer
  @brief		: 发到游戏服务器。
  */
  bool pushMsgToGameServer(uint32_t world_id, int64_t uid, int32_t type, pbc_slice* data);
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
  @func			: sendToClientCanSendLogin
  @brief		: 
  */
  void sendToClientCanSendLogin(uint32_t world_id, uint64_t uid, uint64_t vtcp_id);
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
  yx::Packet buildQueuedMsg(int64_t uid, int32_t type, pbc_slice* data);
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
  //typedef std::set<uint32_t> spawn_world_ids_t;
  //spawn_world_ids_t spawn_world_ids_;
  struct OnlineStatus {
    int num;
    uint64_t updateTime;
  };
  typedef std::map<uint32_t, OnlineStatus> game_server_status_t;
  game_server_status_t game_server_status_;
private:
  // 已登录列表。
  struct login_t {
    uint32_t world_id;
    uint64_t vtcp_id;
    uint64_t m_LoginSession;
    uint64_t m_LoginSessionCreateTime;
    uint64_t swicthWorldSession;
    login_t(uint64_t vtcp_id) {
      this->vtcp_id = vtcp_id;
      world_id = 0;
      m_LoginSession = 0;
      m_LoginSessionCreateTime = 0;
    }
  };
  typedef std::map<uint64_t/*uid*/, login_t> uid_logineds_t;
  uid_logineds_t uid_logineds_;
  struct pending_login {
    uint64_t vtcp_id;
    int      login_count;
  };
  typedef std::map<uint64_t/*uid*/, pending_login> pending_logins2_t;
  pending_logins2_t pending_logins2_;
  struct Game {
    //bool m_GameServerStarted;  // gameServer是否启动
    bool m_GameServerActive;
    uint64_t m_LastBreatheTimestamp;
    Game() {
      //m_GameServerStarted = false;
      m_GameServerActive = false; // 初始化时为false,不允许往RecvClientMsgsQueue里添加消息。只有收到网关心跳后才能发消息
      m_LastBreatheTimestamp = 0;
    }
  };
  typedef std::unordered_map<uint32_t/*world_id*/, Game> games_t;
  games_t games_;
};


// -------------------------------------------------------------------------
#endif /* AGENT_MANAGER_YW_H_ */
