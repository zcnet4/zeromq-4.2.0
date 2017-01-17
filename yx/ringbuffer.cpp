/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\ringbuffer.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-5 22:23
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "ringbuffer.h"
// -------------------------------------------------------------------------
//#define USE_FAST_MEMCPY
#ifdef USE_FAST_MEMCPY
#include "FastMemcpy.h"
#define MEMCPY(dst,src, sz) memcpy_fast((dst), (src), (sz))
#else
#define MEMCPY(dst,src, sz) memcpy((dst), (src), (sz))
#endif // USE_FAST_MEMCPY

namespace yx {

RingBuffer::RingBuffer(uint32_t size)
  : write_index_(0)
  , read_index_(0)
{
  unsigned int powersOfTwo[32] = {
    1,          2,        4,        8,        16,       32,       64,       128,        256,        512, 
    1024,       2048,     4096,     8192,     16384,    32768,    65536,    131072,     262144,     524288, 
    1048576,    2097152,  4194304,  8388608,  16777216, 33554432, 67108864, 134217728,  268435456,  536870912,
    1073741824, 2147483648 };

  int exponent = 0;
  while (powersOfTwo[exponent] < size && exponent < 31)
    exponent++;

   if (size != powersOfTwo[exponent] && exponent > 1) {
     max_size_ = powersOfTwo[exponent/* - 1*/];
     } else {
     max_size_ = size;
     }

   buffer_ = new uint8_t[max_size_];
}

RingBuffer::~RingBuffer()
{
  //buffer_ = nullptr;
  delete buffer_;
}

uint32_t RingBuffer::cast_buf_pos(uint32_t index) {
  return index & (max_size_ - 1);
}

/*
@func			: read_available
@brief		: 返回ringbuffer中实际内存长度
*/
size_t RingBuffer::read_available() const {
  return write_index_.load() - read_index_.load();
}
/*
@func			: write_available
@brief		:
*/
size_t RingBuffer::write_available() const {
  return max_size() - read_available();
}

/*
@func			: write_count
@brief		:
*/
uint32_t RingBuffer::write_count() const {
  return write_count_;
}
/*
@func			: max_size
@brief		: 
*/
uint32_t RingBuffer::max_size() const {
  return max_size_;
}

/*
@func			: write
@brief		: 只能在单一线程中写入。
*/
int RingBuffer::write(const uint8_t* buf, uint16_t buf_size) {
  const size_t avail = write_available();
  if (avail == 0 || sizeof(uint16_t) + buf_size > avail) 
    return -1;
  //
  size_t write_index = write_index_.load();
  uint8_t* p0 = buffer_ + cast_buf_pos(write_index++);
  *p0 = buf_size >> 8;
  uint8_t* p1 = buffer_ + cast_buf_pos(write_index++);
  *p1 = buf_size & 0xFF;
  //
  const size_t write_buf_pos = cast_buf_pos(write_index);
  if (write_buf_pos + buf_size > max_size_) {
    const size_t len0 = max_size_ - write_buf_pos;
    MEMCPY(buffer_ + write_buf_pos, buf, len0);
    MEMCPY(buffer_, buf + len0, buf_size - len0);
  } else {
    // 读memcpy竟比写的memcpy快N倍？？
    MEMCPY(buffer_ + write_buf_pos, buf, buf_size);
  }
  //
  write_index_.fetch_add(sizeof(uint16_t) + buf_size);
  write_count_.fetch_add(1);
  //
  return buf_size;
}

/*
@func			:
@brief		:
*/
int RingBuffer::read(uint8_t* buf, uint16_t buf_size) {
  const size_t avail = read_available();
  if (avail < 2) {
    size_t read_index = read_index_.load();
    size_t write_index = write_index_.load();
    //printf("read avail:%u r:%u w:%u\n ", avail, read_index, write_index);
    return -1;
  }
  //
  size_t read_index = read_index_.load();
  uint8_t* p0 = buffer_ + cast_buf_pos(read_index++);
  uint8_t* p1 = buffer_ + cast_buf_pos(read_index++);
  uint16_t _buf_size = *p0 << 8 | *p1;
  //
  if (_buf_size > 0) {
    const size_t read_buf_pos = cast_buf_pos(read_index);
    if (read_buf_pos + _buf_size < max_size_) {
      MEMCPY(buf, buffer_ + read_buf_pos, _buf_size);
    } else {
      const size_t len0 = max_size_ - read_buf_pos;
      MEMCPY(buf, buffer_ + read_buf_pos, len0);
      MEMCPY(buf + len0, buffer_, _buf_size - len0);
    }
  }
  //
  read_index_.fetch_add(sizeof(uint16_t) + _buf_size);
  write_count_.fetch_sub(1);
  //
  return _buf_size;
}

}; // namespace yx

#if 0
#include <thread>
#include <chrono>
#include <windows.h>
#define LOOP_COUNT 10000000  
#define CAPACITY 10000000
yx::RingBuffer rb(CAPACITY);
  int count = 0;
  
  void test_write_full() {
    const int kBufSize = 64 * 1024;
    uint8_t buf[kBufSize] = { 0 };

    clock_t start = clock();

    for (int i = 0; i < LOOP_COUNT;) {
      int r = rb.write(buf, 32768);
      if (-1 != r) {
        i++;
      } else {
        break;
      }
    } // for
    clock_t end = clock();
    printf("[test_write_full]\nCAPACITY:%d\ncost:%dms\n", CAPACITY, end - start);
  }
  void test_read_complete() {
    const int kBufSize = 64 * 1024;
    uint8_t buf[kBufSize] = { 0 };
    clock_t start = clock();

    while (1) {
      int si = rb.read(buf, kBufSize);
      if (si >= 0) {
        ++count;
      } else {
        break;
      }

      if (count >= LOOP_COUNT) {
        break;
      }
    }

    clock_t end = clock();  
    printf("[test_write_full]\nCAPACITY:%d\ncost:%dms\n", CAPACITY, end - start);
  }

  void test_write()
  {
    const int kBufSize = 64 * 1024;
    uint8_t buf[kBufSize] = { 0 };
    std::chrono::microseconds m1 = std::chrono::microseconds(1);
    for (int i = 0; i < LOOP_COUNT;) {
      int r = rb.write(buf, 32768);
      if (-1 != r){
        i++;
      } else {
        //std::this_thread::sleep_for(m1);
        //printf("P sleep\n");
        Sleep(0);
      }
      int r2 = rb.write(buf, 15200);
      if (-1 != r2) {
        i++;
      } else {
        //std::this_thread::sleep_for(m1);
        //printf("P sleep\n");
        Sleep(0);
      }
    }
  }

  void test_read()
  {
    const int kBufSize = 64 * 1024;
    uint8_t buf[kBufSize] = { 0 };
    while (1) {
      int si = rb.read(buf, kBufSize);
      if (si >= 0) {
        ++count;
      } else {
        //std::this_thread::sleep_for(m1);
        Sleep(0);
        //printf("C sleep\n");
      }

      if (count >= LOOP_COUNT) {
        break;
      }
    }
  }
  void test_ringbuffer(){
    /*test_write_full();
    test_read_complate();
    return;*/
    clock_t start = clock();



    std::thread *nc_t = new std::thread((&test_read));
    std::thread *np_t = new std::thread((&test_write));
    nc_t->join();
    np_t->join();

    clock_t end = clock();
    printf("[test_nonblocking]\nCAPACITY:%d\ncost:%dms\n", CAPACITY, end - start);
    printf("count:%d\n", count);

    delete nc_t;
    delete np_t;
  }
  class A{
  public:
    A()
    {
      test_ringbuffer();
    }
  };
  static A a;
#endif

// -------------------------------------------------------------------------
