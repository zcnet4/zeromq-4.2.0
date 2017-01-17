/* -------------------------------------------------------------------------
//    FileName		:	D:\yx_code\yx\yx\packet_view.cpp
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-12-14 22:23
//    Description	:    
//
// -----------------------------------------------------------------------*/
#include "packet_view.h"
#include <memory.h>
// -------------------------------------------------------------------------

namespace yx
{

PacketView::PacketView()
  : read_offset_(0)
  , write_offset_(0)
  , buf_size_(0)
  , buf_(nullptr)
{
  
}

PacketView::PacketView(const Packet& packet)
  : packet_(packet)
{
  reset();
}

PacketView::~PacketView()
{

}

void PacketView::reset() {
  buf_size_       = packet_.buf_size();
  buf_            = packet_.mutable_buf();
  read_offset_    = 0;
  write_offset_   = 0;
}

void PacketView::inset(uint16_t left, uint16_t right/* = 0*/) {
  buf_ += left;
  buf_size_ -= (left + right);
}

//
void PacketView::read_uint16(uint16_t* n) {
  _read_u16(buf_ + read_offset_, n);
  read_offset_ += sizeof(uint16_t);
}

void PacketView::read_uint32(uint32_t* n) {
  _read_u32(buf_ + read_offset_, n);
  read_offset_ += sizeof(uint32_t);
}

bool PacketView::read_string(uint8_t* str, int* str_size) {
  uint16_t _str_size = 0;
  read_uint16(&_str_size);
  if (read_offset_ + _str_size > buf_size_)
    return false;
  //
  memcpy(str, buf_ + read_offset_, _str_size);
  read_offset_ += _str_size;
  //
  return true;
}

bool PacketView::read_header(uint16_t* cmd, uint32_t* session, uint32_t* uid) {
  if (buf_size_ < sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t))
    return false;
  //
  uint16_t read_offset(read_offset_);
  read_offset_ = 0;
  read_uint16(cmd);
  read_uint32(session);
  read_uint32(uid);
  read_offset_ = read_offset;
  //
  return true;
}

void PacketView::write_uint16(uint16_t n) {
  _write_u16(buf_ + write_offset_, n);
  write_offset_ += sizeof(uint16_t);
}

void PacketView::write_uint32(uint32_t n) {
  _write_u32(buf_ + write_offset_, n);
  write_offset_ += sizeof(uint32_t);
}

bool PacketView::write_string(const uint8_t* str, int str_size) {
  if (write_offset_ + sizeof(uint16_t) + str_size > buf_size_) 
    return false;
  //
  write_uint16(str_size);
  memcpy(buf_ + write_offset_, str, str_size);
  write_offset_ += str_size;
  //
  return true;
}

bool PacketView::write_header(uint16_t cmd, uint32_t session, uint32_t uid) {
  if (sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uid) > buf_size_)
    return false;
  uint16_t write_offset(write_offset_);
  write_offset_ = 0;
  write_uint16(cmd);
  write_uint32(session);
  write_uint32(uid);
  write_offset_ = write_offset;
  //
  return true;
}

const uint8_t* _read_str(const uint8_t* buf, uint8_t* str, int* str_size) {
  uint16_t _str_size = 0;
  buf = _read_u16(buf, &_str_size);
  memcpy(str, buf, _str_size);
  if (str_size) {
    *str_size = _str_size;
  } else {
    str[_str_size] = '\0';
  }
  return buf + _str_size;
}

uint8_t* _write_str(uint8_t* buf, const uint8_t* str, int str_size) {
  buf = _write_u16(buf, str_size);
  memcpy(buf, str, str_size);
  return buf + str_size;
}

}; // namespace yx

// -------------------------------------------------------------------------