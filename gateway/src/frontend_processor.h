/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\frontend_processor.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 14:25
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef FRONTEND_PROCESSOR_H_
#define FRONTEND_PROCESSOR_H_
#include <unordered_map>
#include "yx/processor.h"
#include "yx/id_allocator.h"
#include "info_trace.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// FrontendProcessor，前端处理器。与客户端保持连接。
class FrontendProcessor 
  : public yx::Processor 
{
public:
  FrontendProcessor();
  ~FrontendProcessor();
public:
  /*
  @func			: Start
  @brief		:
  */
  virtual bool Start();
  /*
  @func			: Stop
  @brief		: 
  */
  virtual void Stop();
protected:
  /*
  @func			: ProcessTcpData
  @brief		:
  */
  virtual void ProcessTcpData(uint64_t fd, uint16_t op, yx::Packet& packet);
  /*
  @func			: ProcessTcpConnect
  @brief		:
  */
  virtual void ProcessTcpConnect(uint64_t fd, uint16_t op, yx::Packet& packet);
  /*
  @func			: ProcessTcpClose
  @brief		:
  */
  virtual void ProcessTcpClose(uint64_t fd, uint16_t op, yx::Packet& packet);
  /*
  @func			: ProcessTcpInput
  @brief		:
  */
  virtual void ProcessTcpInput(uint16_t op, uint64_t id, yx::Packet& packet);
private:
  /*
  @func			: OpenVTcpId
  @brief		: 
  */
  uint64_t OpenVTcpId(uint64_t forward_id, bool auto_create = true);
  /*
  @func			: CloseVTcpId
  @brief		: 
  */
  void CloseVTcpId(uint64_t forward_id);
private:
  typedef std::unordered_map<uint64_t/*forward_id*/, uint64_t/*vtcp_id*/> id2id_t;
  id2id_t forward_ids_;
  id2id_t vtcp_ids_;
  yx::IDAllocator vtcpid_allocator_;          //虚拟tcp_id分配器。
  InfoTrace info_trace_;
};


// -------------------------------------------------------------------------
#endif /* FRONTEND_PROCESSOR_H_ */
