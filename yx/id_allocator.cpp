/* -------------------------------------------------------------------------
//    FileName		:	D:\yx_code\yx\yx\id_allocator.cpp
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-11-14 23:31
//    Description	:    
//
// -----------------------------------------------------------------------*/
#include "id_allocator.h"
#include "uv.h"
#include "atomic.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// IDAllocator
IDAllocator::IDAllocator()
  : machine_id_(0)
  , epoch_(1482391966891)  //从1970年到2016-12-22 15:34的毫秒数。by ZC
  , last_time_(0)
  , sequence_(0)
{

}

IDAllocator::~IDAllocator()
{

}

void IDAllocator::set_machine_id(uint8_t machine_id) {
  machine_id_ = (machine_id & 0x0FF) << 5 | worker_id();
}

uint8_t IDAllocator::machine_id() const {
  return (machine_id_ >> 5) & 0x0FF;
}

void IDAllocator::set_worker_id(uint8_t worker_id) {
  machine_id_ = (machine_id_ & 0x01FE0) | (worker_id & 0x01F);
}

uint8_t IDAllocator::worker_id() const {
  return machine_id_ & 0x01F;
}

uint8_t IDAllocator::ExtractWorkerId(uint64_t id) {
  return static_cast<uint8_t>(id & 0x01F);
}
/*
@func			: generate
@brief		:
*/
uint64_t IDAllocator::generate(uint64_t now_time) {
  // twitter snowflake算法(改进)
  // 64       63----------23--------13-------5-------0
  // 符号位   |  40位时间   |10位自增码|8位机器码|5位内部码|
  /*
  2 ^ 8 = 256个机器码
  2 ^ 5 = 32 个内部码
  2 ^ 40 =1099511627776毫秒
  一年的毫秒3600 * 24 * 365 * 1000 = 31536000000毫秒
  40位时间可以用34.8年。
  */
  int64_t new_id = 0;
restart:
  uint64_t time = 0;
  if (now_time > last_time_) {
    time = now_time - epoch_;
    last_time_ = now_time;
  } else if (last_time_ == now_time) {
    int seq = ATOM_INC(&sequence_) & 0x3FF;
    if (sequence_ >= 0x400) {
      sequence_ = 0;
      now_time++;
      goto restart;
    }
    // 中间10位是sequenceID
    new_id |= seq << 13;
    time = now_time - epoch_;
  } else {
    // if (last_time_ > now_time) 
    now_time++;
    goto restart;
  }
  // 保留后40位时间
  new_id |= (time & 0x0FFFFFFFFFF) << 23;
  // 低13位是机器ID
  new_id |= machine_id_ & 0x1FFF;
  //
  return new_id;
  // twitter snowflake算法
  // 64       63--------------22---------12---------0
  // 符号位   |     41位时间   |10位机器码|12位自增码|
/*
  int64_t new_id = 0;
restart:
  uint64_t time = 0;
  if (now_time > last_time_) {
    time = now_time - epoch_;
    last_time_ = now_time;
  } else if (last_time_ == now_time) {
    // 最后12位是sequenceID
    new_id |= ATOM_INC(&sequence_) & 0xFFF;
    if (sequence_ == 0x1000) {
      sequence_ = 0;
      now_time++;
      goto restart;
    }
    time = now_time - epoch_;
  } else {
    // if (last_time_ > now_time) 
    time = last_time_ - epoch_;
  }
  // 保留后41位时间
  new_id |= (time & 0x1FFFFFFFFFF) << 22;
  // 中间10位是机器ID
  new_id |= (machine_id_ & 0x3FF) << 12;
  //
  return new_id;
*/
}

}; // namespace yx
// -------------------------------------------------------------------------