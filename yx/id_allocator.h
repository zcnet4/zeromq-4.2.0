/* -------------------------------------------------------------------------
//    FileName		:	D:\yx_code\yx\yx\id_allocator.h
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-11-14 23:30
//    Description	:    
//
// -----------------------------------------------------------------------*/
#ifndef ID_ALLOCATOR_H_
#define ID_ALLOCATOR_H_
#include <stdint.h>
#include "yx_export.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// IDAllocator
class YX_EXPORT IDAllocator
{
public:
  IDAllocator();
  IDAllocator(const IDAllocator&) = delete;
  IDAllocator& operator=(const IDAllocator&) = delete;
  ~IDAllocator();
  static uint8_t ExtractWorkerId(uint64_t id);
public:
  /*
  @func			: generate
  @brief		:
  */
  uint64_t generate(uint64_t now_time);
  //
  void set_machine_id(uint8_t machine_id);
  uint8_t machine_id() const;
  void set_worker_id(uint8_t worker_id);
  uint8_t worker_id() const;
private:
  uint16_t machine_id_;
  uint64_t epoch_;
  uint64_t last_time_;
  int/*int32_t*/ sequence_;
};

}; // namespace yx
// -------------------------------------------------------------------------
#endif /* ID_ALLOCATOR_H_ */