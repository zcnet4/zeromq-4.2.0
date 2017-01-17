/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\packet_queue.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-7 9:48
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "packet_queue.h"

// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
//
PacketQueue::PacketQueue()
{

}

PacketQueue::~PacketQueue()
{

}

/*
@func			: Push
@brief		:
*/
void PacketQueue::Push(uint16_t op_type, uint64_t fd2id, const Packet& packet) {
  Box box;
  box.op_type = op_type;
  box.fd2id.fd = fd2id;
  box.packet = packet;
  //
  lock_.lock();
  queue_.push(box);
  lock_.unlock();
}


/*
@func			: Pop
@brief		:
*/
bool PacketQueue::Pop(uint16_t& op_type, uint64_t& fd2id, Packet& packet) {
  Box box;
  lock_.lock();
  if (queue_.pop(box)) {
    lock_.unlock();
    op_type = box.op_type;
    fd2id = box.fd2id.fd;
    packet = box.packet;
    return true;
  }
  //
  lock_.unlock();
  return false;
}

/*
@func			: PopUnsafe
@brief		:
*/
bool PacketQueue::PopUnsafe(Box& box) {
  return queue_.pop(box);
}


/*
@func			: Size
@brief		:
*/
int PacketQueue::Size() {
  int size = 0;
  //
  lock_.lock();
  size = queue_.size();
  lock_.unlock();
  //
  return size;
}

/*
@func			: SizeUnsafe
@brief		:
*/
int PacketQueue::SizeUnsafe() {
  return queue_.size();
}

/*
@func			: Swap
@brief		:
*/
void PacketQueue::Swap(PacketQueue& src) {
  lock_.lock();
  queue_.swap(src.queue_);
  lock_.unlock();
}

}; // namespace yx


// -------------------------------------------------------------------------
