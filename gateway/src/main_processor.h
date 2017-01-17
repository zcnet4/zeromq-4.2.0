/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\main_processor.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-8 10:51
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef MAIN_PROCESSOR_H_
#define MAIN_PROCESSOR_H_
#include <memory>
#include "yx/processor.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
class Runner;
class YWServer;
//////////////////////////////////////////////////////////////////////////
// MainProcessor£¬Ö÷´¦ÀíÆ÷¡£
class MainProcessor
  : public yx::Processor
{
public:
  MainProcessor();
  ~MainProcessor();
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
  std::unique_ptr<YWServer> yw_server_;
};



// -------------------------------------------------------------------------
#endif /* MAIN_PROCESSOR_H_ */
