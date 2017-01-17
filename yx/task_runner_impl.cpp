/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\task_runner_impl.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-3 18:24
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "task_runner_impl.h"
#include "loop_impl.h"
#include "logging.h"
// -------------------------------------------------------------------------

namespace yx {

TaskRunnerImpl::TaskRunnerImpl(LoopImpl* loop)
{
  loop_id_ = loop->loop_id();
	uv_async_init(loop->uv_loop(), 
    &wakeup_req_,
#ifdef NDEBUG
    nullptr
#else
    &TaskRunnerImpl::wakeup_cb
#endif // NDEBUG
    );
	wakeup_req_.data = this;
}

TaskRunnerImpl::~TaskRunnerImpl()
{

}

void TaskRunnerImpl::wakeup_cb(uv_async_t* handle) {
	// nothing
}

/*
@func			: Wakeup
@brief		: 呼醒Loop。
*/
void TaskRunnerImpl::Wakeup() {
  if (uv_is_active((uv_handle_t*)&wakeup_req_))
    uv_async_send(&wakeup_req_);
  else
    LOG(ERROR) << "loop destroyed!!!";
}

/*
@func			: PostTask
@brief		:
*/
void TaskRunnerImpl::PostTask(const Closure& task) {

  if (task) {
    lock_.lock();
    task_queue_.push(task);
    lock_.unlock();
    // 及时唤醒Loop。by ZC. 2016-11-2 18:20
    Wakeup();
  }
}

/*
@func			: RunTasks
@brief		: 执行任务。
*/
void TaskRunnerImpl::RunTasks() {
  // 当工作队列为空时，才装载任务队列。
  if (0 == work_queue_.size()) {
    lock_.lock();
    task_queue_.swap(work_queue_);
    lock_.unlock();
  }
  //
  uv_loop_t* uv_loop = wakeup_req_.loop;
  //uint64_t t0 = uv_now(uv_loop);
  //(void)t0;
  //
	Closure task;
  while (0 == uv_loop->stop_flag && work_queue_.pop(task)) {
		task();
    // next
  }
  //uv_update_time(uv_loop);
  //uint64_t t1 = uv_now(uv_loop);
  //(void)t1;

  // 是否再次呼醒。
  if (0 == work_queue_.size()) {
    lock_.lock();
    int task_queue_size = task_queue_.size();
    lock_.unlock();
    if (task_queue_size) {
      Wakeup();
    }
  } else {
    Wakeup();
  }
}

/*
@func			: WillDestroy
@brief		:
*/
void TaskRunnerImpl::WillDestroy() {
  // 删除未执行的任务。
  Closure task;
clear:
  while (work_queue_.pop(task)) {
    task = nullptr;
    // next
  }
  lock_.lock();
  task_queue_.swap(work_queue_);
  lock_.unlock();
  if (work_queue_.size())
    goto clear;
  // 
  uv_close((uv_handle_t*)&wakeup_req_, nullptr);
}

}; // namespace yx

// -------------------------------------------------------------------------
