/* -------------------------------------------------------------------------
//	FileName		��	D:\yx_code\yx\yx\memory_pool.cpp
//	Creator			��	(zc) <zcnet4@gmail.com>
//	CreateTime	��	2015-2-28 16:44
//	Description	��	
//
// -----------------------------------------------------------------------*/
#include "memory_pool.h"
#include <assert.h>
#include <memory>
#include "allocator.h"
// -------------------------------------------------------------------------

/* MEM_ALIGN() is only to be used to align on a power of 2 boundary */
// �������ӽ�size��boundary����������������ͨ�������size��СΪ�������ɣ���boundary����뱣֤Ϊ2�ı�����
#define MEM_ALIGN(size, boundary)     (((size) + ((boundary) - 1)) & ~((boundary) - 1)) 
#define MEM_ALIGNMENT                 sizeof(void*)
#define MIN_SLICE_SIZE                8192   /*8KB*/
#define MAX_SLICE_SIZE                32768  /*32KB*/

namespace yx {

struct _MEM_CHUNK {
  _MEM_CHUNK* next;
  char free_flag[1];                  //���_MEM_SLICE free״̬��
};
static const uint32_t kChunkSize = MEM_ALIGN(sizeof(_MEM_CHUNK*), MEM_ALIGNMENT);

// �ڴ���Ƭ
struct _MEM_SLICE {
  union {
    _MEM_SLICE* next;
    void* data;
  };
};


MemoryPool::MemoryPool(const char* name, uint32_t slices_size, uint32_t slices_count/* = 0*/)
  : name_(name ? name : "")
  , chunks_(nullptr)
  , free_slices_(nullptr)
  , slice_count_(1)
  , slice_size_(MEM_ALIGN(slices_size, MEM_ALIGNMENT))
  , allocated_total_(0)
  , free_total_(0)
{
  // ����32Kʱ��chunk������Ƭ��
  if (slice_size_ >= MAX_SLICE_SIZE) {
    const_cast<uint32_t&>(slice_count_) = 1;
  } else if (slice_size_ < 256) {
    // ����Ƭslice_size̫С�������һ���ڴ���䡣
    const_cast<uint32_t&>(slice_count_) = (MIN_SLICE_SIZE - kChunkSize) / slice_size_;
  } else {
    const_cast<uint32_t&>(slice_count_) = (MAX_SLICE_SIZE - kChunkSize) / slice_size_;
  }
  //
  if (0 != slices_count && slice_count_ < slices_count) {
    //::MessageBox(NULL, 0, L"CPool", 0);
    const_cast<uint32_t&>(slice_count_) = slices_count;
  }
}

MemoryPool::~MemoryPool()
{
  int32_t slice_total = 0;
  for (_MEM_CHUNK* _chunk = chunks_; _chunk; _chunk = _chunk->next) {
    slice_total += slice_count_;
  }

  if (allocated_total_ > 0) {
    assert(false);
    int i = 0;
    i++;
  }
  if (allocated_total_ + free_total_ != slice_total) {
    assert(false);
    int i = 0;
    i++;
  }
  //
  clear();
}

/*
@func			: allocate
@brief		: 
*/
void* MemoryPool::allocate() {
  if (free_slices_ == nullptr) {
    _MEM_CHUNK* _chunk = (_MEM_CHUNK*)(yx_malloc(kChunkSize + slice_count_ * slice_size_));
    if (_chunk == nullptr) {
      return nullptr;
    }
    _chunk->next = chunks_;
    chunks_ = _chunk;
    // ��ʼ�������ڴ���Ƭ�б�
    _MEM_SLICE* _cursor = reinterpret_cast<_MEM_SLICE*>(reinterpret_cast<char*>(_chunk)+kChunkSize);
    for (uint32_t i = 0; i < slice_count_; ++i) {
      _cursor->next = free_slices_;
      free_slices_ = _cursor;
      ++free_total_;
      // next
      _cursor = reinterpret_cast<_MEM_SLICE*>(reinterpret_cast<char*>(_cursor) + slice_size_);
    }
  }
  //
  _MEM_SLICE* _slice = free_slices_;
  free_slices_ = free_slices_->next;
  ++allocated_total_;
  --free_total_;
  //
  return (void*)_slice;
}

/*
@func			: deallocate
@brief		: 
*/
void MemoryPool::deallocate(void* p) {
  assert(p);
#if 0
  _MEM_SLICE* _cursor = reinterpret_cast<_MEM_SLICE*>(reinterpret_cast<char*>(_chunk)+kChunkSize);
  while (_cursor) {
    if (p <= _cursor || p >= reinterpret_cast<char*>(_cursor) + slice_count_ * slice_size_) {
      assert(false);
      return;
    }
    // next
    _cursor = _cursor->next;
  }
#endif
  //
  _MEM_SLICE* _slice = (_MEM_SLICE*)p;
  _slice->next = free_slices_;
  free_slices_ = _slice;
  --allocated_total_;
  ++free_total_;
}

/*
@func			: clear
@brief		: 
*/
void MemoryPool::clear() {
  while (chunks_) {
    _MEM_CHUNK* _chunk = chunks_;
    chunks_ = chunks_->next;
    yx_free(_chunk);
  }
  //
  chunks_ = nullptr;
  free_slices_ = nullptr;
}

}; // namespace yx
// -------------------------------------------------------------------------
