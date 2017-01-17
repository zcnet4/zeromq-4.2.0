/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\ringbuffer.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-5 22:23
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_
#include "yx_export.h"
#include <atomic>
#include <stdint.h>
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
//
class YX_EXPORT RingBuffer {
public:
  RingBuffer(uint32_t size);
  RingBuffer(const RingBuffer&) = delete;
  RingBuffer& operator=(const RingBuffer&) = delete;
  //RingBuffer(RingBuffer&&) = delete;
  ~RingBuffer();
public:
  /*
  @func			: read_available
  @brief		: 返回ringbuffer中实际内存长度
  */
  size_t read_available() const;
  /*
  @func			: write_available
  @brief		: 
  */
  size_t write_available() const;
  /*
  @func			: empty
  @brief		: 
  */
  bool empty(void) const {
    return 0 == read_available();
  }
  /*
  @func			: write_count
  @brief		: 
  */
  uint32_t write_count() const;
  /*
  @func			: max_size
  @brief		:
  */
  uint32_t max_size() const;
  /*
  @func			: write
  @brief		: 只能在单一线程中写入。
  */
  int write(const uint8_t* buf, uint16_t buf_size);
  /*
  @func			: read
  @brief		: 
  */
  int read(uint8_t* buf, uint16_t buf_size);
private:
  uint32_t cast_buf_pos(uint32_t val);
private:
  std::atomic<size_t> write_index_;
  //cache line是CPU在做缓存的时候有个最小缓存单元，在同一个单元内的数据被同时被加载到缓存中，充分利用cache line可以大大降低数据读写的延迟。
  //错误利用cache line也会导致缓存不同替换，反复失效。
  char padding_[64 - sizeof(size_t)]; //强制write_index跟read_index_在不同CPU单元中。
  std::atomic<size_t> read_index_;
  //
  uint8_t* buffer_;
  uint32_t max_size_;
  std::atomic<uint32_t> write_count_;       //ringbuffer中buf个数。
};

}; // namespace yx
// -------------------------------------------------------------------------
#endif /* RINGBUFFER_H_ */
