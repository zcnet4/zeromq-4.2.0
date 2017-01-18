/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\recv_packet.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-27 11:33
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "recv_cache.h"
#include "uv.h"
#include "memory_pool.h"
#include <algorithm>
#include "logging.h"
// -------------------------------------------------------------------------
namespace yx {

//////////////////////////////////////////////////////////////////////////
//
RecvCache::RecvCache()
  : packet_size_cache_(0)
  , readable_offset_(0)
  , recv_total_size_(0)
  , buf_block_head_(nullptr)
  , buf_block_tail_(nullptr)
{
  static_assert(sizeof(readable_offset_) == sizeof(kBufSize), "sizeof(readable_offset_) != sizeof(kBufSize)");
  static_assert(sizeof(recv_total_size_) == sizeof(uint32_t), "sizeof(recv_total_size_) != sizeof(uint32_t)");
}

RecvCache::~RecvCache()
{

}

int RecvCache::GetBlockSize() {
  return sizeof(buf_block) + kBufSize;
}

/*
@func			: alloc
@brief		:
*/
void RecvCache::alloc(MemoryPool* pool, uv_buf_t* buf) {
  int buf_size = 0;
  uint8_t* ptr = alloc_buf_block(pool, &buf_block_tail_, &buf_size);
  if (nullptr == buf_block_head_) {
    buf_block_head_ = buf_block_tail_;
  }
  //
  buf->base = (char*)ptr;
  buf->len = buf_size;
}

/*
@func			: recv_size
@brief		:
*/
void RecvCache::recv_size(int nread) {
  buf_block_tail_->used += nread;
  recv_total_size_ += nread;
}

/*
@func			: clear
@brief		:
*/
void RecvCache::clear(MemoryPool* pool) {
  for (buf_block* cur = buf_block_head_; cur;) {
    buf_block* free_block = cur;
    cur = cur->next;
    pool->deallocate(free_block);
  }
  buf_block_head_ = nullptr;
  buf_block_tail_ = nullptr;
  recv_total_size_ = 0;
  readable_offset_ = 0;
}

/*
@func			: alloc_buf_block
@brief		:
*/
uint8_t* RecvCache::alloc_buf_block(MemoryPool* pool, RecvCache::buf_block** ref_bb, int* alloc_buf_block_size) {
  buf_block* bb = *ref_bb;
  if (nullptr == bb || bb->used >= kBufSize) {
    // 分配新块。
    bb = (buf_block*)pool->allocate();
    bb->next = nullptr;
    bb->used = 0;
    bb->ref_count = 1;
    bb->padding = 0xFFFF;
    //
    if (*ref_bb) {
      (*ref_bb)->next = bb;
    }
    *ref_bb = bb;
  }
  //
  *alloc_buf_block_size = kBufSize - bb->used;
  //
  return (reinterpret_cast<uint8_t*>(bb + 1) + bb->used);
}

/*
@func			: decode_header
@brief		:
*/
bool RecvCache::decode_header(MemoryPool* pool, uint16_t* packet_size) {
  if (recv_total_size_ < sizeof(uint16_t))
    return false;
  // peek
  if (0 == packet_size_cache_) {
    buf_block*& bb = buf_block_head_;
    uint16_t readable_size = bb->used - readable_offset_;
    if (readable_size >= sizeof(uint16_t)) {
      uint8_t* ptr = reinterpret_cast<uint8_t*>(bb + 1) + readable_offset_;
      packet_size_cache_ = ptr[0] << 8 | ptr[1];
    } else if (readable_size == 1) {
      uint8_t* ptr = reinterpret_cast<uint8_t*>(bb + 1) + readable_offset_;
      packet_size_cache_ = ptr[0] << 8;
      ptr = reinterpret_cast<uint8_t*>(bb->next + 1) + 0;
      packet_size_cache_ |= ptr[0];
    } else {
      uint8_t* ptr = reinterpret_cast<uint8_t*>(bb->next + 1) + readable_offset_;
      packet_size_cache_ = ptr[0] << 8 | ptr[1];
    }
  }
  // read
#ifdef PACKET_SIZE_CONTAINS_PACKET_SIZE
  if (packet_size_cache_ <= recv_total_size_) {
#else
  if (packet_size_cache_ + sizeof(uint16_t) <= recv_total_size_) {
#endif // PACKET_SIZE_CONTAINS_PACKET_SIZE
    buf_block*& bb = buf_block_head_;
    uint16_t readable_size = bb->used - readable_offset_;
    if (readable_size >= sizeof(uint16_t)) {
      readable_offset_ += sizeof(uint16_t);
      recv_total_size_ -= sizeof(uint16_t);
    } else if (readable_size == 1) {
      bb->ref_count--;
      buf_block_head_ = bb->next;
      recycle_buf_block(pool, bb);
      readable_offset_ = sizeof(uint8_t);
      recv_total_size_ -= sizeof(uint16_t);
    } else {
      bb->ref_count--;
      buf_block_head_ = bb->next;
      recycle_buf_block(pool, bb);
      readable_offset_ = sizeof(uint16_t);
      recv_total_size_ -= sizeof(uint16_t);
    }
#ifdef PACKET_SIZE_CONTAINS_PACKET_SIZE
    *packet_size = packet_size_cache_ - sizeof(uint16_t);
#else
    *packet_size = packet_size_cache_;
#endif // PACKET_SIZE_CONTAINS_PACKET_SIZE
    return true;
  } else {
    return false;
  }
}

/*
@func			: decode
@brief		: 当decode_header返回true时，才能调用decode
*/
void RecvCache::decode(MemoryPool* pool, uint8_t* packet, uint16_t packet_size) {
  DCHECK(packet_size == packet_size_cache_);
  if (0 == packet_size) return;
  //
  buf_block* scoped_buf_block_head = buf_block_head_;
  read_buf_block(&buf_block_head_, packet, packet_size);
  if (nullptr == buf_block_head_)
    buf_block_tail_ = nullptr;
  recycle_buf_block(pool, scoped_buf_block_head);
  //
  packet_size_cache_ = 0;
}

void RecvCache::read_buf_block(RecvCache::buf_block** bb, uint8_t* out_buffer, int read_size) {
  DCHECK(read_size > 0) << "read_size Must be greater than zero。";
  //
  for (buf_block* cur = *bb; cur; cur = cur->next) {
    int len = std::min<int>(cur->used - readable_offset_, read_size);
    if (len > 0) {
      uint8_t* ptr = reinterpret_cast<uint8_t*>(cur + 1) + readable_offset_;
      memcpy(out_buffer, ptr, len);
      read_size -= len;
      recv_total_size_ -= len;
      out_buffer += len;
      // 读完。
      if (read_size == 0) {
        readable_offset_ = readable_offset_ + len;
        if (readable_offset_ == kBufSize/*刚好满就指向下一块。*/) {
          *bb = cur->next;
          readable_offset_ = 0;
          --cur->ref_count; // 跑到下一个块，说明本块需要收回。
        }
        break;
      }
    }
    //
    *bb = cur->next;
    readable_offset_ = 0;
    --cur->ref_count; // 跑到下一个块，说明本块需要收回。
  } // for
}

/*
@func			:  recycle_buf_block
@brief		:
*/
void RecvCache::recycle_buf_block(MemoryPool* pool, buf_block* bb) {
  for (buf_block* cur = bb; cur;) {
    if (0 == cur->ref_count) {
      buf_block* free_block = cur;
      // next
      cur = cur->next;
      free_block->padding = 0xABCD;
      pool->deallocate(free_block);
    } else {
      break;
    }
  }
}
}; // namespace yx
// -------------------------------------------------------------------------
