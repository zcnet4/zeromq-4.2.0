/* -------------------------------------------------------------------------
//    FileName		:	D:\yx_code\yx\yx\packet_view.h
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-12-14 22:23
//    Description	:    
//
// -----------------------------------------------------------------------*/
#ifndef PACKET_VIEW_H_
#define PACKET_VIEW_H_
#include "yx_export.h"
#include "packet.h"
// -------------------------------------------------------------------------
namespace yx
{

class YX_EXPORT PacketView
{
public:
  PacketView();
  PacketView(const Packet& packet);
  ~PacketView();
public:
  uint8_t* buf() const {
    return buf_;
  }
  uint16_t buf_size() const {
    return buf_size_;
  }
  void reset();
  void inset(uint16_t left, uint16_t right = 0);
  //
  uint16_t write_remaining() {
    return buf_size_ - write_offset_;
  }
  uint16_t read_remaining() {
    return buf_size_ - read_offset_;
  }
  //
  void read_uint16(uint16_t* n);
  void read_uint32(uint32_t* n);
  bool read_string(uint8_t* str, int* str_size);
  bool read_header(uint16_t* cmd, uint32_t* session, uint32_t* uid);
  //
  void write_uint16(uint16_t n);
  void write_uint32(uint32_t n);
  bool write_string(const uint8_t* str, int str_size);
  bool write_header(uint16_t cmd, uint32_t session, uint32_t uid);
private:
  uint16_t          read_offset_;
  uint16_t          write_offset_;
  uint16_t          buf_size_;
  uint8_t*          buf_;
  yx::Packet        packet_;
};


inline const uint8_t* _read_u16(const uint8_t* buf, uint16_t* n) {
  *n = buf[0] << 8 | buf[1];
  return buf + sizeof(uint16_t);
}

inline const uint8_t* _read_u32(const uint8_t* buf, uint32_t* n) {
  *n = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
  return buf + sizeof(uint32_t);
}

inline const uint8_t* _read_u64(const uint8_t* buf, uint64_t* n) {
  uint32_t l = buf[4] << 24 | buf[5] << 16 | buf[6] << 8 | buf[7];
  uint32_t h = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
  *n = h;
  *n <<= 32;
  *n |= l;
  return buf + sizeof(uint64_t);
}

YX_EXPORT const uint8_t* _read_str(const uint8_t* buf, uint8_t* str, int* str_size);

inline uint8_t* _write_u16(uint8_t* buf, uint16_t n) {
  buf[0] = (n >> 8) & 0xff;
  buf[1] = n & 0xff;
  return buf + sizeof(uint16_t);
}

inline uint8_t* _write_u32(uint8_t* buf, uint32_t n) {
  buf[0] = (n >> 24) & 0xff;
  buf[1] = (n >> 16) & 0xff;
  buf[2] = (n >> 8) & 0xff;
  buf[3] = n & 0xff;
  return buf + sizeof(uint32_t);
}

inline uint8_t* _write_u64(uint8_t* buf, uint64_t n) {
  buf[0] = (n >> 56) & 0xff;
  buf[1] = (n >> 48) & 0xff;
  buf[2] = (n >> 40) & 0xff;
  buf[3] = (n >> 32) & 0xff;
  buf[4] = (n >> 24) & 0xff;
  buf[5] = (n >> 16) & 0xff;
  buf[6] = (n >> 8) & 0xff;
  buf[7] = n & 0xff;
  return buf + sizeof(uint64_t);
}

YX_EXPORT uint8_t* _write_str(uint8_t* buf, const uint8_t* str, int str_size);

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* PACKET_VIEW_H_ */