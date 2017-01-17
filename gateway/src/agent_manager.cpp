/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\agent_manager.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-11 17:40
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "agent_manager.h"
#include "runner.h"
#include <algorithm>
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// AgentManager
AgentManager::AgentManager()
{

}

AgentManager::~AgentManager()
{

}

/*
@func			: CreateAgent
@brief		:
*/
Agent* AgentManager::CreateAgent(uint64_t vtcp_id) {
  id2agents_t::iterator iter(id2agents_.find(vtcp_id));
  if (id2agents_.end() != iter) {
    return iter->second;
  }
  Agent* agent = NewAgent();
  agent->set_vtcp_id(vtcp_id);
  //
  id2agents_.emplace(id2agents_t::value_type(agent->vtcp_id(), agent));
  //
  return agent;
}

/*
@func			: GetAgent
@brief		:
*/
Agent* AgentManager::GetAgent(uint64_t vtcp_id) {
  id2agents_t::iterator iter(id2agents_.find(vtcp_id));
  if (id2agents_.end() != iter) {
    return iter->second;
  }
  return nullptr;
}


/*
@func			: CloseAgent
@brief		:
*/
void AgentManager::CloseAgent(uint64_t vtcp_id) {
  auto iter = id2agents_.find(vtcp_id);
  if (id2agents_.end() != iter) {
    id2agents_t::mapped_type del = std::move(iter->second);
    id2agents_.erase(iter);
    /*if (del->real()) {
      runner_->CloseTcp(del->tcp_id());
    }*/
    delete del;
  }
}


/*
@func			: for_each
@brief		:
*/
void AgentManager::for_each(const std::function<void(Agent*)>& cb) {
  std::for_each(id2agents_.begin(), id2agents_.end(), [&cb](id2agents_t::value_type& v) {
    cb(v.second);
  });
}
// -------------------------------------------------------------------------
