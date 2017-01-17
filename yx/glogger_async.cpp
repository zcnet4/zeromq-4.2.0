/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\glogger_async.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-24 15:35
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "glogger_async.h"
#include "logging.h"
#include "uv.h"
#include "loop.h"
#include "loop_impl.h"
#include "allocator.h"
//#include "ringbuffer.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
class GLoggerAsync
  : public google::base::Logger
{
public:
  virtual void Flush() {
    raw_logger_->Flush();
  }
  virtual google::uint32 LogSize() {
    return raw_logger_->LogSize();
  }
  static void on_uv_work_cb(uv_work_t* req) {
    uint8_t* ptr = (uint8_t*)req->data;
    google::base::Logger* raw_logger = reinterpret_cast<google::base::Logger*>(*(reinterpret_cast<int*>(ptr)));
    ptr += sizeof(google::base::Logger*);
    bool* force_flush = reinterpret_cast<bool*>(ptr);
    ptr += sizeof(bool);
    time_t* timestamp = reinterpret_cast<time_t*>(ptr);
    ptr += sizeof(time_t);
    int* message_len = reinterpret_cast<int*>(ptr);
    ptr += sizeof(int);
    //
    raw_logger->Write(*force_flush, *timestamp, (const char*)ptr, *message_len);
  }
  static void on_uv_after_work_cb(uv_work_t* req, int status) {
    yx_free(req->data);
    yx::LoopImpl::free_uv_req(req);
  }
  virtual void Write(bool force_flush,
    time_t timestamp,
    const char* message,
    int message_len) {
    yx::Loop* loop = yx::Loop::current();
    uv_work_t* work = (uv_work_t*)loop->loop_impl()->alloc_uv_req();
    //
    void* log_data = yx_malloc(sizeof(google::base::Logger*) + sizeof(bool) + sizeof(time_t) + sizeof(int) + message_len);
    uint8_t* ptr = (uint8_t*)log_data;
    //
    memcpy(ptr, &raw_logger_, sizeof(google::base::Logger*));
    ptr += sizeof(google::base::Logger*);
    memcpy(ptr, &force_flush, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &timestamp, sizeof(time_t));
    ptr += sizeof(time_t);
    memcpy(ptr, &message_len, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, message, message_len);
    work->data = log_data;
    //
    uv_queue_work(loop->uv_loop(), work, on_uv_work_cb, on_uv_after_work_cb);
  }
  GLoggerAsync() : raw_logger_(nullptr) {}
  void set_raw_logger(google::base::Logger* raw_logger){
    raw_logger_ = raw_logger;
  }
private:
  google::base::Logger* raw_logger_;
  //yx::RingBuffer        message_ring_;
};


GLoggerAsync gloggers[google::NUM_SEVERITIES];

void async_glogger(int level) {
  if (&gloggers[level] != google::base::GetLogger(level)) {
    gloggers[level].set_raw_logger(google::base::GetLogger(level));
    google::base::SetLogger(level, &gloggers[level]);
  }
}

void* async_glogger_get(int level) {
  return &gloggers[level];
}

void async_glogger_set(int level, void* l) {
  gloggers[level].set_raw_logger((google::base::Logger*)l);
}

// -------------------------------------------------------------------------
