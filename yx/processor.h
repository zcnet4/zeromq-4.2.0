/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\processor.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 16:56
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef PROCESSOR_H_
#define PROCESSOR_H_
#include <memory>
#include "yx_export.h"
#include "loop.h"
#include "tcp_delegate.h"
#include "socket_manager.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
//
class Packet;
class PacketQueue;
class TaskRunner;
//////////////////////////////////////////////////////////////////////////
// Processor
class YX_EXPORT Processor
  : private yx::ITcpDelegate
{
public:
  Processor();
  virtual ~Processor();
  class YX_EXPORT Handle {
  public:
    /*
    @func			: Send
    @brief		: 
    */
    void Send(uint16_t op_type, uint64_t fd2id, const Packet& packet);
  private:
    std::shared_ptr<yx::PacketQueue> task_queue_;
    std::shared_ptr<yx::TaskRunner>  task_runner_;
    friend class Processor;
  };
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
  @func			: handle
  @brief		: 
  */
  Handle handle();
  /*
  @func			: loop
  @brief		: 
  */
  yx::Loop* loop() const {
    return loop_;
  }
  /*
  @func			: socket_manager
  @brief		: 
  */
  yx::SocketManager* socket_manager() const {
    return socket_manager_;
  }
protected:
  /*
  @func			: ProcessQueue
  @brief		:
  */
  void ProcessQueue();
  /*
  @func			: ProcessPacket
  @brief		:
  */
  virtual void ProcessPacket(uint64_t fd2id, uint16_t op_type, yx::Packet& packet);
  /*
  @func			: ProcessTcpData
  @brief		:
  */
  virtual void ProcessTcpData(uint64_t fd, uint16_t op, yx::Packet& packet) = 0;
  /*
  @func			: ProcessTcpConnect
  @brief		:
  */
  virtual void ProcessTcpConnect(uint64_t fd, uint16_t op, yx::Packet& packet) = 0;
  /*
  @func			: ProcessTcpClose
  @brief		:
  */
  virtual void ProcessTcpClose(uint64_t fd, uint16_t op, yx::Packet& packet) = 0;
  /*
  @func			: ProcessTcpInput
  @brief		:
  */
  virtual void ProcessTcpInput(uint16_t op, uint64_t id, yx::Packet& packet) = 0;
private:
  virtual void OnTcpConnect(yx::Tcp* connect_tcp, int err);
  virtual void OnTcpAccpet(yx::Tcp* tcp, yx::Tcp* accpet_tcp, int err);
  virtual void OnTcpRead(yx::Tcp* tcp, yx::Packet& packet);
  virtual void OnTcpClose(yx::Tcp* tcp);
private:
  yx::Loop* loop_;
  yx::SocketManager* socket_manager_;
  std::shared_ptr<yx::PacketQueue> task_queue_;
  std::shared_ptr<yx::PacketQueue> work_queue_;
  std::shared_ptr<yx::TaskRunner>  task_runner_;
};

}

// -------------------------------------------------------------------------
#endif /* PROCESSOR_H_ */
