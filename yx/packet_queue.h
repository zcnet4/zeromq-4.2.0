/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\packet_queue.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-7 9:48
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef PACKET_QUEUE_H_
#define PACKET_QUEUE_H_
#include <stdint.h>
#include "yx_export.h"
#include "yx_queue.h"
#include "packet.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// PacketQueue
class YX_EXPORT PacketQueue
{
public:
  PacketQueue();
  PacketQueue(const PacketQueue&) = delete;
  PacketQueue& operator = (const PacketQueue&) = delete;
  ~PacketQueue();
  struct Box {
    uint16_t op_type;
    union {
      uint64_t fd;                     //文件描述符。
      uint64_t id;                     //input类型为id。
    } fd2id;
    yx::Packet packet;
  };
public:
  /*
  @func			: Push
  @brief		: 
  */
  void Push(uint16_t op_type, uint64_t fd2id, const Packet& packet);
  /*
  @func			: Pop
  @brief		: 
  */
  bool Pop(uint16_t& op_type, uint64_t& fd2id, Packet& packet);
  /*
  @func			: PopUnsafe
  @brief		:
  */
  bool PopUnsafe(Box& box);
  /*
  @func			: Size
  @brief		: 
  */
  int Size();
  /*
  @func			: SizeUnsafe
  @brief		:
  */
  int SizeUnsafe();
  /*
  @func			: Swap
  @brief		: 
  */
  void Swap(PacketQueue& src);
private:
  yx::SpinLock                         lock_;
  yx::QueueT<Box, 1024, yx::NonLock>   queue_;
};

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* PACKET_QUEUE_H_ */
