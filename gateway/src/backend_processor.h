/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\backend_processor.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 14:25
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef BACKEND_PROCESSOR_H_
#define BACKEND_PROCESSOR_H_
#include "zmq_server.h"
#include "forward_server.h"
#include "skynet_server.h"
#include "yx/processor.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
class Runner;
//////////////////////////////////////////////////////////////////////////
// BackendProcessor，后端处理器。与后端服务保持连接。
class BackendProcessor
  : public yx::Processor
{
public:
  BackendProcessor();
  ~BackendProcessor();
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
  Runner*       runner_;
  ZmqServer     zmq_server_;
  ForwardServer forward_server_;
  SkynetServer  skynet_server_;
};


// -------------------------------------------------------------------------
#endif /* BACKEND_PROCESSOR_H_ */
