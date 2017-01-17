/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\gateway.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-1 14:32
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef GATEWAY_H_
#define GATEWAY_H_
#include "runner.h"
#include "yx/yx_timer.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
class FrontendProcessor;
class BackendProcessor;
class MainProcessor;
//////////////////////////////////////////////////////////////////////////
// Gateway
class Gateway
{
public:
  Gateway();
  ~Gateway();
public:
  /*
  @func			: Start
  @brief		: 
  */
  bool Start();
  /*
  @func			: Run
  @brief		: 
  */
  void Run();
  /*
  @func			: Stop
  @brief		: 
  */
  bool Stop();
  /*
  @func			: StopSafe
  @brief		: 
  */
  void StopSafe();
protected:
  /*
  @func			: start_server
  @brief		: 
  */
  void start_processor();
  /*
  @func			: dump_memory
  @brief		: 
  */
  void dump_memory();
private:
  bool stopping_;
  Runner runner_;
  MainProcessor* main_processor_;
  // 网关需要两个物理网卡，分区与前后端保持连接。by ZC. 2016-11-4 15:41.
  FrontendProcessor* frontend_processor_;
  BackendProcessor* backend_processor_;
  yx::Timer* dump_memory_timer_;
};

namespace yx {
  class Packet;
}
void SendToMain(uint64_t fd, uint16_t op_type, const yx::Packet& packet);
void InputToMain(uint16_t op_type, uint64_t id, const yx::Packet& packet);
void InputToFrontend(uint16_t op_type, uint64_t id, const yx::Packet& packet);
void InputToBackend(uint16_t op_type, uint64_t id, const yx::Packet& packet);

// -------------------------------------------------------------------------
#endif /* GATEWAY_H_ */
