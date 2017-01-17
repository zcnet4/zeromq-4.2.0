/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\runner.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 12:03
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "runner.h"
#include "yx/id_allocator.h"
#include "yx/tcp.h"
// -------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// Runner
Runner::Runner()
{

}

Runner::~Runner()
{

}

/*
@func			: Start
@brief		:
*/
void Runner::Start() {
  for (int i = MAIN + 1; i < ID_COUNT; i++) {
    runners_[i - 1].Start();
  }
}

/*
@func			: Run
@brief		:
*/
void Runner::Run() {
  main_runner_.Run();
}

/*
@func			: Stop
@brief		:
*/
void Runner::Stop() {
  for (int i = MAIN + 1; i < ID_COUNT; i++) {
    runners_[i - 1].Stop();
  }
  main_runner_.Quit();
}

/*
@func			: PostTask
@brief		:
*/
void Runner::PostTask(ID id, const yx::Closure& task) {
  if (id == MAIN) {
    main_runner_.PostTask(task);
  } else if (id > MAIN && id < ID_COUNT) {
    runners_[id - 1].PostTask(task);
  }
}

/*
@func			: PostTask
@brief		:
*/
void Runner::PostTask(uint16_t loop_id, const yx::Closure& task) {
  if (main_runner_.loop_id() == loop_id) {
    main_runner_.PostTask(task);
  } else {
    for (int i = MAIN + 1; i < ID_COUNT; i++) {
      if (loop_id == runners_[i - 1].loop_id())
        runners_[i - 1].PostTask(task);
    }
  }
}

/*
@func			: CloseTcp
@brief		:
*/
void Runner::CloseTcp(uint64_t tcp_id) {
  uint16_t loop_id = yx::IDAllocator::ExtractWorkerId(tcp_id);
  //
  PostTask(loop_id, [tcp_id] {
    /*yx::Tcp* tcp = yx::Loop::current()->FromTcp(tcp_id);
    if (tcp) {
      tcp->Close();
    }*/
  });
}

/*
@func			: main_runner
@brief		:
*/
std::shared_ptr<yx::TaskRunner> Runner::main_runner() {
  return main_runner_.task_runner();
}

/*
@func			: now_mainLoop
@brief		:
*/
uint64_t Runner::now_mainLoop() const {
  return main_runner_.ticks_now();
}

// -------------------------------------------------------------------------
