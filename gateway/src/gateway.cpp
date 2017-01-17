/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\gateway.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-1 14:32
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "gateway.h"
#include "gateway_config.h"
#include "runner.h"
#include "frontend_processor.h"
#include "backend_processor.h"
#include "main_processor.h"
#include "yx/logging.h"
#include "yx/yx_util.h"
#include <signal.h>
#include "yx/allocator.h"
// -------------------------------------------------------------------------
static Gateway* g_gateway = nullptr;
//////////////////////////////////////////////////////////////////////////
// Gateway
Gateway::Gateway()
  : stopping_(false)
  , main_processor_(nullptr)
  , frontend_processor_(nullptr)
  , backend_processor_(nullptr)
  , dump_memory_timer_(nullptr)
{
  g_gateway = this;
}

Gateway::~Gateway()
{
  g_gateway = nullptr;
} 

void sigint_handler(int sig) {
  LOG(WARNING) << "recv signal " << sig << ", set m_killed to true";
  if (g_gateway) {
    g_gateway->StopSafe();
  }
}
/*
@func			: Start
@brief		:
*/
bool Gateway::Start() {
  signal(SIGINT, sigint_handler);
  signal(SIGTERM, sigint_handler);
  //
  runner_.Start();
  runner_.PostTask(Runner::MAIN, std::bind(&Gateway::start_processor, this));
  //
  return true;
}

void Gateway::Run() {
  runner_.Run();
}

/*
@func			: start_processor
@brief		:
*/
void Gateway::start_processor() {
  bool success = false;
  void* mwait = yx::util::multwait_new(4);
  //
  runner_.PostTask(Runner::WORK, [this, mwait, &success]() {
    main_processor_ = new MainProcessor();
    success = main_processor_->Start();
    LOG(WARNING) << "Main Processor Startup" << (success ? " Complete!!!" : "Failed!!!");
    yx::util::multwait_wait(mwait);
  });
  //
  //在FRONTEND上调用Frontend Processor
  runner_.PostTask(Runner::FRONTEND, [this, mwait]() {
    frontend_processor_ = new FrontendProcessor();
    frontend_processor_->Start();
    LOG(WARNING) << "Frontend Processor Startup Complete!!!";
    yx::util::multwait_wait(mwait);
  });
  //在BACKEND上调用Backend Processor
  runner_.PostTask(Runner::BACKEND, [this, mwait]() {
    backend_processor_ = new BackendProcessor();
    backend_processor_->Start();
    LOG(WARNING) << "Backend Processor Startup Complete!!!";
    yx::util::multwait_wait(mwait);
  });
  //
  yx::util::multwait_wait(mwait);
  if (success) {
    dump_memory();
    LOG(WARNING) << "Gateway Startup Complete!!!";
  } else {
    LOG(ERROR) << "Gateway Startup Failed!!!";
    Stop();
  }
}


/*
@func			: Stop
@brief		:
*/
bool Gateway::Stop() {
  if (stopping_) return true;
  stopping_ = true;
  //
  runner_.PostTask(Runner::FRONTEND, [this]() {
    if (frontend_processor_) {
      frontend_processor_->Stop();
      delete frontend_processor_;
      frontend_processor_ = nullptr;
    }
  });
  runner_.PostTask(Runner::BACKEND, [this]() {
    if (backend_processor_) {
      backend_processor_->Stop();
      delete backend_processor_;
      backend_processor_ = nullptr;
    }
  });
  runner_.PostTask(Runner::WORK, [this]() {
    if (main_processor_) {
      main_processor_->Stop();
      delete main_processor_;
      main_processor_ = nullptr;
    }
  });
  //
  if (dump_memory_timer_) {
    dump_memory_timer_->Stop();
    dump_memory_timer_->Close();
  }
  runner_.Stop();
  //
  return true;
}

/*
@func			: StopSafe
@brief		:
*/
void Gateway::StopSafe() {
  runner_.PostTask(Runner::MAIN, [this](){
    this->Stop();
  });
}

/*
@func			: dump_memory
@brief		:
*/
void Gateway::dump_memory() {
  dump_memory_timer_ = yx::Timer::New(yx::Loop::current());
  dump_memory_timer_->set_cb([](yx::Timer* t) {
    size_t current_allocated_bytes = yx_current_allocated_bytes();
    size_t heap_size = yx_heap_size();
    LOG(INFO) << "dump_memory heap_size:" << heap_size << " current_allocated_bytes:" << current_allocated_bytes;
    t->Start(5 * 60 * 1000);
  });
  dump_memory_timer_->Start(5 * 60 * 1000);
}

// -------------------------------------------------------------------------
