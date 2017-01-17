/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\tcp_delegate.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-26 21:21
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef TCP_DELEGATE_H_
#define TCP_DELEGATE_H_
#include <stdint.h>
#include "packet.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// Tcp
class Tcp;
struct ITcpDelegate {
  virtual ~ITcpDelegate(){}
  virtual void OnTcpConnect(Tcp* connect_tcp, int err) = 0;
  virtual void OnTcpAccpet(Tcp* tcp, Tcp* accpet_tcp, int err) = 0;
  virtual void OnTcpRead(Tcp* tcp, Packet& packet) = 0;
  virtual void OnTcpClose(Tcp* tcp) = 0;
};
//////////////////////////////////////////////////////////////////////////
// Udp
class Udp;
struct IUdpDelegate {
  virtual ~IUdpDelegate(){}
  virtual void OnUdpConnect(Udp* connect_udp, int err) = 0;
  virtual void OnUdpAccpet(Udp* udp, Udp* accpet_udp, int err) = 0;
  virtual void OnUdpRead(Udp* udp, Packet& packet) = 0;
  virtual void OnUdpClose(Udp* udp) = 0;
};

const uint16_t kTCP_MASK    = 0xF000;
const uint16_t kTCP_CONNECT = 0x1000;
const uint16_t kTCP_ACCEPT  = 0x2000;
const uint16_t kTCP_DATA    = 0x3000;
const uint16_t kTCP_ERROR   = 0x4000;
const uint16_t kTCP_CLOSE   = 0x5000;
const uint16_t kTCP_INPUT   = 0x6000;

struct IProcessPacket {
  /*
  @func			: ProcessPacket
  @brief		:
  */
  virtual void ProcessPacket(uint64_t fd2id, uint16_t op_type, yx::Packet& packet) = 0;
};

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* TCP_DELEGATE_H_ */
