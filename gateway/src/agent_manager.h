/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\agent_manager.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-11 17:40
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef AGENT_MANAGER_H_
#define AGENT_MANAGER_H_
#include <unordered_map>
#include <functional>
#include "agent.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
class Runner;
//////////////////////////////////////////////////////////////////////////
// AgentManager
class AgentManager
{
public:
  AgentManager();
  ~AgentManager();
public:
  /*
  @func			: CreateAgent
  @brief		:
  */
  Agent* CreateAgent(uint64_t vtcp_id);
  /*
  @func			: GetAgent 
  @brief		: 
  */
  Agent* GetAgent(uint64_t vtcp_id);
  /*
  @func			: CloseAgent
  @brief		: 
  */
  void CloseAgent(uint64_t vtcp_id);
protected:
  /*
  @func			: NewAgent
  @brief		:
  */
  virtual Agent* NewAgent() = 0;
  /*
  @func			: for_each
  @brief		: 
  */
  void for_each(const std::function<void(Agent*)>& cb);
private:
  typedef std::unordered_map<uint64_t/*vtcp_id*/, Agent*> id2agents_t;
  id2agents_t id2agents_;
};


// -------------------------------------------------------------------------
#endif /* AGENT_MANAGER_H_ */
