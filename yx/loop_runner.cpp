/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\loop_runner.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-1 17:15
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "loop_runner.h"
#include "loop.h"
#include "uv.h"
#include <memory.h> // for memset
// -------------------------------------------------------------------------

namespace yx {

//////////////////////////////////////////////////////////////////////////
// LoopRunner
LoopRunner::LoopRunner()
  : loop_id_(0)
{
  status_.store(EStatus::Stop);
  memset(thread_storage_, 0, sizeof(thread_storage_));
}

LoopRunner::~LoopRunner()
{
  Stop();
}

struct ThreadArg {
  LoopRunner* wroker;
  uv_sem_t sem;
};
/*
@func			: Start
@brief		:
*/
void LoopRunner::Start() {
  status_.store(EStatus::Start);
  ThreadArg arg;
  arg.wroker = this;
  uv_sem_init(&arg.sem, 0);
  uv_thread_create((uv_thread_t*)thread_storage_, &LoopRunner::thread_run, &arg);
  // Wait for the thread to complete initialization.
  uv_sem_wait(&arg.sem);
  uv_sem_destroy(&arg.sem);
}

void LoopRunner::thread_run(void* arg) {
  uv_sem_t* sem = &reinterpret_cast<ThreadArg*>(arg)->sem;
  LoopRunner* wroker = reinterpret_cast<ThreadArg*>(arg)->wroker;
  //
	yx::Loop loop;
  wroker->loop_id_ = loop.loop_id();
  wroker->task_runner_ = loop.task_runner();
  wroker->status_.store(EStatus::Running);
  uv_sem_post(sem);
  //
  wroker->OnRun(&loop);
  wroker->status_.store(EStatus::Shutdown);
}

/*
@func			: OnRun
@brief		:
*/
void LoopRunner::OnRun(Loop* loop) {
  loop->Run();
}

/*
@func			: Stop
@brief		:
*/
void LoopRunner::Stop() {
  EStatus exp = EStatus::Running;
  if (status_.compare_exchange_strong(exp, EStatus::Stop)) {
    task_runner_->PostTask([]() {
      yx::Loop::current()->Quit();
    });
    uv_thread_join((uv_thread_t*)thread_storage_);
  }
}

/*
@func			: PostTask
@brief		:
*/
void LoopRunner::PostTask(const Closure& task) {
  if (task_runner_)
    task_runner_->PostTask(task);
}

}; // namespace yx

// -------------------------------------------------------------------------
