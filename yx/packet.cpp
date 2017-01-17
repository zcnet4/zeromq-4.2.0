/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\packet.cpp
//	Creator			:	(zc)
//	CreateTime	:	2016-11-17 15:03
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "packet.h"
#include <memory>
#include "atomic.h"
#include "build/compiler_specific.h"
#include "allocator.h"
#include "logging.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// Packet
Packet::Packet()
  : header_(nullptr)
{
  static_assert(sizeof(Packet) == sizeof(Packet::Header*), "");
}

Packet::Packet(uint16_t buf_size) 
  : header_(nullptr)
{
  resize(buf_size);
  header_->ref_count = 1;
  header_->buf_size = buf_size;
  header_->param_ = 0;
}

Packet::Packet(const Packet& src)
  : header_(src.header_)
{
  ref();
}

Packet& Packet::operator = (const Packet& src) {
  const_cast<Packet&>(src).ref();
  unref();
  header_ = src.header_;
  return *this;
}

Packet::~Packet()
{
  unref();
}

void Packet::ref() {
  if (header_) {
#ifdef ARCH_CPU_LITTLE_ENDIAN
    CHECK(header_->ref_count) << "ref_count is greater than 65535";
    ATOM_INC(reinterpret_cast<int32_t*>(&header_->ref_count));
#else
#error "yx::Packet Not support"
#endif // ARCH_CPU_LITTLE_ENDIAN
  }
}

void Packet::unref() {
#ifdef ARCH_CPU_LITTLE_ENDIAN
  if (header_) {
    ATOM_DEC(reinterpret_cast<int32_t*>(&header_->ref_count));
    if (header_->ref_count == 0)
      yx_free(release());
  }
#else
#error "yx::Packet Not support"
#endif // ARCH_CPU_LITTLE_ENDIAN
}

uint16_t Packet::offset() const {
  return static_cast<uint16_t>(header_->param_);
}
/*
@func			: release
@brief		:
*/
void* Packet::release() {
  Header* release_header = header_;
#ifdef OS_WIN
  ATOM_CAS_POINTER((unsigned int*)&header_, (int)release_header, NULL);
#else
  ATOM_CAS_POINTER(&header_, release_header, NULL);
#endif // OS_WIN
  return release_header;
}


/*
@func			: reset
@brief		:
*/
void Packet::reset(void* header) {
  unref();
  header_ = static_cast<Header*>(header);
}


/*
@func			: Resize
@brief		:
*/
void Packet::resize(uint16_t buf_size) {
  void* p = yx_realloc(header_, sizeof(Header) + buf_size);
  header_ = reinterpret_cast<Header*>(p);
  header_->buf_size = buf_size;
}

}; // namespace yx
// -------------------------------------------------------------------------
