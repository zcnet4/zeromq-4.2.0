/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\agent.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-9 13:28
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef AGENT_H_
#define AGENT_H_
#include <stdint.h>
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
class AgentManager;
//////////////////////////////////////////////////////////////////////////
// Agent
class Agent 
{
public:
  Agent();
  virtual ~Agent();
public:
  uint64_t vtcp_id() const { return vtcp_id_;}
  bool auth() const { return auth_; }
  void set_auth(bool auth) { auth_ = auth; }
private:
  void set_vtcp_id(uint64_t vtcp_id) { vtcp_id_ = vtcp_id; }
private:
  uint64_t vtcp_id_;          // 虚拟tcpid。
  bool     auth_;
  friend class AgentManager;
};

// -------------------------------------------------------------------------
#endif /* AGENT_H_ */
