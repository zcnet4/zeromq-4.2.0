/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\processor.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 16:56
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "processor.h"
#include "packet_queue.h"
#include "task_runner.h"
#include "loop.h"
#include "logging.h"
#include "tcp.h"
#include "udp.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// Processor
Processor::Processor()
  : loop_(nullptr)
  , socket_manager_(nullptr)
  , task_queue_(new yx::PacketQueue())
  , work_queue_(new yx::PacketQueue())
{

}

Processor::~Processor()
{

}

/*
@func			: Start
@brief		:
*/
bool Processor::Start() {
  loop_ = yx::Loop::current();
  socket_manager_ = loop_->socket_manager();
  task_runner_ = loop_->task_runner();
  //
  loop_->Watch(std::bind(&Processor::ProcessQueue, this));
  socket_manager()->set_deleagte(this);
  //
  return true;
}

/*
@func			: Stop
@brief		:
*/
void Processor::Stop() {
  task_runner_ = nullptr;
  loop()->Watch(yx::Closure());
  socket_manager()->Shutdown();
}

void Processor::OnTcpConnect(yx::Tcp* connect_tcp, int err) {
  yx::Packet packet(sizeof(int));
  //
  memcpy(packet.mutable_buf(), &err, sizeof(int));
  connect_tcp->ReadStart();
  //
  ProcessPacket(connect_tcp->tcp_id(), kTCP_CONNECT | connect_tcp->tcp_type(), packet);
  if (err != 0) {
    loop()->socket_manager()->Close(connect_tcp->tcp_id());
  }
}

void Processor::OnTcpAccpet(yx::Tcp* tcp, yx::Tcp* accpet_tcp, int err) {
  accpet_tcp->set_delegate(this);
  accpet_tcp->set_tcp_type(tcp->tcp_type() & ~yx::kTCP_MASK);
  accpet_tcp->set_nodelay(true);
  accpet_tcp->ReadStart();
  //
  {
    char peer_host[32];
    accpet_tcp->get_peerhost(peer_host);
    LOG(INFO) << "New client from : " << peer_host;
  }
  //
  yx::Packet packet(sizeof(uint64_t) + sizeof(int));
  uint8_t* buf = packet.mutable_buf();
  *reinterpret_cast<uint64_t*>(buf) = accpet_tcp->tcp_id();
  buf += sizeof(uint64_t);
  memcpy(buf, &err, sizeof(int));
  //
  ProcessPacket(tcp->tcp_id(), kTCP_ACCEPT | tcp->tcp_type(), packet);
}

void Processor::OnTcpRead(yx::Tcp* tcp, yx::Packet& packet) {
  ProcessPacket(tcp->tcp_id(), kTCP_DATA | tcp->tcp_type(), packet);
}

void Processor::OnTcpClose(yx::Tcp* tcp)  {
  yx::Packet packet(0);
  ProcessPacket(tcp->tcp_id(), kTCP_CLOSE | tcp->tcp_type(), packet);
}

/*
@func			: ProcessQueue
@brief		:
*/
void Processor::ProcessQueue() {
  // 当工作队列为空时，才装载任务队列。
  if (0 == work_queue_->SizeUnsafe()) {
    task_queue_->Swap(*work_queue_);
  }
  //
  PacketQueue::Box box;
  while (work_queue_->PopUnsafe(box)) {
    ProcessPacket(box.fd2id.fd, box.op_type, box.packet);
  }
}

/*
@func			: ProcessPacket
@brief		:
*/
void Processor::ProcessPacket(uint64_t fd2id, uint16_t op_type, yx::Packet& packet) {
  uint16_t type = op_type & yx::kTCP_MASK;
  uint16_t op = op_type & ~yx::kTCP_MASK;
  //
  switch (type) {
  case yx::kTCP_CONNECT:
    ProcessTcpConnect(fd2id, op, packet);
    break;
  case yx::kTCP_ACCEPT:
    break;
  case yx::kTCP_DATA:
    ProcessTcpData(fd2id, op, packet);
    break;
  case yx::kTCP_CLOSE:
  case yx::kTCP_ERROR:
    ProcessTcpClose(fd2id, op, packet);
    break;
  case yx::kTCP_INPUT:
    ProcessTcpInput(op, fd2id, packet);
    break;
  default:
    LOG(WARNING) << "Unrecognized Type:" << type;
  }
}

/*
@func			: GetHandle
@brief		:
*/
Processor::Handle Processor::handle() {
  Processor::Handle q;
  q.task_queue_ = task_queue_;
  q.task_runner_ = task_runner_;
  return q;
}

/*
@func			: Send
@brief		:
*/
void Processor::Handle::Send(uint16_t op_type, uint64_t fd2id, const Packet& packet) {
  DCHECK((op_type & kTCP_MASK) != 0) << "The op type must not be null";
  task_queue_->Push(op_type, fd2id, packet);
  task_runner_->Wakeup();
}

}

// -------------------------------------------------------------------------
