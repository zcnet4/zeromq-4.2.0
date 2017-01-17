/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\packet.h
//	Creator			:	(zc)
//	CreateTime	:	2016-11-17 15:03
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef PACKET_H_
#define PACKET_H_
#include <stdint.h>
#include "yx_export.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// Packet
class YX_EXPORT Packet
{
  struct Header;
public:
  /*
  @func		: unspecified_bool_type
  @brief		: 增加bool值判定。by ZC. 2012-10-19 10:49.
  */
  typedef Header* Packet::* unspecified_bool_type;
  operator unspecified_bool_type() const {
    return header_ ? &Packet::header_ : nullptr;
  }
public:
  Packet();
  Packet(uint16_t buf_size);
  Packet(const Packet& src);
  Packet& operator=(const Packet& src);
  ~Packet();
public:
  /*
  @func			: resize
  @brief		: 
  */
  void resize(uint16_t buf_size);
  /*
  @func			: release
  @brief		: 
  */
  void* release();
  /*
  @func			: reset 
  @brief		: 
  */
  void reset(void*);

  const uint8_t* buf() const {
    return reinterpret_cast<const uint8_t*>(header_ + 1);
  }
  uint8_t* mutable_buf(){
    return reinterpret_cast<uint8_t*>(header_ + 1);
  }
  uint16_t buf_size() const { 
    return header_->buf_size; 
  }
  uint32_t size() const { 
    return sizeof(Header) + header_->buf_size; 
  }
  const uint64_t& param() const {
    return header_->param_;
  }
  uint64_t& mutable_param() {
    return header_->param_;
  }
  void set_param(uint64_t param) const {
    header_->param_ = param;
  }
  uint16_t offset() const;
protected:
  /*
  @func			: ref
  @brief		:
  */
  void ref();
  /*
  @func			: unref
  @brief		:
  */
  void unref();
private:
#pragma pack(push)
#pragma pack(1)
  struct Header {
    uint16_t ref_count;             // 引用计数。
    uint16_t buf_size;              // 数据包缓冲区大小。
    uint64_t param_;                // 参数，64bit为能容纳一个指针大小。
  };
#pragma pack(pop)
  Header* header_;
};

}; // namespace yx

// -------------------------------------------------------------------------
#endif /* PACKET_H_ */
