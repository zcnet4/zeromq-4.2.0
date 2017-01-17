/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\main_processor.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-8 10:51
//	Description	:
//
// -----------------------------------------------------------------------*/
#include "main_processor.h"
#include "yx/logging.h"
#include "gateway_config.h"
#include "gateway_util.h"
#include "runner.h"
#include "op_defs.h"
#include "yw_server.h"
#include "netproto.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
static yx::Processor::Handle kMainProcessor;
void SendToMain(uint64_t op_id, uint16_t op_type, const yx::Packet& packet) {
  kMainProcessor.Send(op_type, op_id, packet);
}
void InputToMain(uint16_t op_type, uint64_t id, const yx::Packet& packet) {
  kMainProcessor.Send(yx::kTCP_INPUT | (op_type & ~yx::kTCP_MASK), id, packet);
}
//////////////////////////////////////////////////////////////////////////
// MainProcessor£¬Ö÷´¦ÀíÆ÷¡£
MainProcessor::MainProcessor()
{

}

MainProcessor::~MainProcessor()
{

}

/*
@func			: Start
@brief		:
*/
bool MainProcessor::Start() {
  bool success = yx::Processor::Start();
  kMainProcessor = this->handle();
  //
#ifdef YX_YW
  yw_server_.reset(new YWServer());
  success = yw_server_->Start(loop());
#endif
  return success;
}

/*
@func			: Stop
@brief		:
*/
void MainProcessor::Stop() {
#ifdef YX_YW
  yw_server_->Stop();
#endif
  yx::Processor::Stop();
}

/*
@func			: ProcessTcpData
@brief		:
*/
void MainProcessor::ProcessTcpData(uint64_t fd, uint16_t op, yx::Packet& packet) {
  
}

/*
@func			: ProcessTcpConnect
@brief		:
*/
void MainProcessor::ProcessTcpConnect(uint64_t fd, uint16_t op, yx::Packet& packet) {

}

/*
@func			: ProcessTcpClose
@brief		:
*/
void MainProcessor::ProcessTcpClose(uint64_t fd, uint16_t op, yx::Packet& packet) {
  switch (op) {
  case TCP_OP::YW_C2F: {
    uint64_t vtcp_id = fd;
    yw_server_->CloseAgent(vtcp_id);
  } break;
  }
}

/*
@func			: ProcessTcpInput
@brief		:
*/
void MainProcessor::ProcessTcpInput(uint16_t op, uint64_t id, yx::Packet& packet) {
  switch (op)
  {
  case TCP_OP::YW_F2M: {
    Agent* agent = yw_server_->CreateAgent(id);
    yw_server_->processClientMsg(agent, packet);
   } break;
  case TCP_OP::YW_B2M: {
#ifdef YX_YW
    const uint8_t* buf = packet.buf();
    uint32_t uid = 0, type = 0;
    buf = yx::_read_u32(buf, &uid);
    buf = yx::_read_u32(buf, &type);
    uint16_t buf_size = packet.buf_size() - (buf - packet.buf());
    yw_server_->processGameMsg(uid, type, buf, buf_size, packet);
#endif // YX_YW
  } break;
  default:
    LOG(WARNING) << "Unrecognized Type:" << op;
    break;
  }
}

// -------------------------------------------------------------------------
