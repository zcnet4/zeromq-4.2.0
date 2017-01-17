/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\recv_cache.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-27 11:33
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef RECV_CACHE_H_
#define RECV_CACHE_H_
#include <stdint.h>
// -------------------------------------------------------------------------
struct uv_buf_t;
namespace yx {
//////////////////////////////////////////////////////////////////////////
//
class MemoryPool;
//////////////////////////////////////////////////////////////////////////
// RecvCache������հ�ʱ�ڴ�Ƶ���������ڴ�ռ�����⡣
class RecvCache {
public:
  RecvCache();
  ~RecvCache();
  static const uint16_t kBufSize = 16 * 1024;
  static const uint16_t kMaxSize = 64 * 1024 - 1;
  static int GetBlockSize();
public:
  /*
  @func			: alloc
  @brief		: 
  */
  void alloc(MemoryPool* pool, uv_buf_t* buf);
  /*
  @func			: recv_size
  @brief		: 
  */
  void recv_size(int nread);
  /*
  @func			: decode_header
  @brief		: 
  */
  bool decode_header(MemoryPool* pool, uint16_t* packet_size);
  /*
  @func			: decode
  @brief		: ��decode_header����trueʱ�����ܵ���decode
  */
  void decode(MemoryPool* pool, uint8_t* packet, uint16_t packet_size);
  /*
  @func			: clear
  @brief		: 
  */
  void clear(MemoryPool* loop);
private:
#pragma pack(push)
#pragma pack(1)
  struct buf_block {
    buf_block* next;                       // sizeof(next) = sizeof(int64)
    uint16_t used;
    uint16_t ref_count;                    // �����ü�����Ϊ0���ͷš�
    uint16_t padding;
  };
#pragma pack(pop)
  uint8_t* alloc_buf_block(MemoryPool* pool, buf_block** bb, int* buf_block_size);
  void read_buf_block(buf_block** bb, uint8_t* out_buffer, int read_size);
  void recycle_buf_block(MemoryPool* pool, buf_block* bb);
private:
  uint16_t    packet_size_cache_;       // ���ݰ����Ȼ�������
  uint16_t    readable_offset_;
  uint32_t    recv_total_size_;         // �������ܳ��ȡ�recv_total_size_�ᳬ��64K��
  buf_block*  buf_block_head_;
  buf_block*  buf_block_tail_;
};

}; // namespace yx
// -------------------------------------------------------------------------
#endif /* RECV_CACHE_H_ */
