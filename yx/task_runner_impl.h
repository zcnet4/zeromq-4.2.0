/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\task_runner_impl.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-3 18:24
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef TASK_RUNNER_IMPL_H_
#define TASK_RUNNER_IMPL_H_
#include "task_runner.h"
#include "yx_queue.h"
#include "uv.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// 
class LoopImpl;
//////////////////////////////////////////////////////////////////////////
// TaskRunnerImpl
class TaskRunnerImpl
	: public TaskRunner
{
public:
  TaskRunnerImpl(LoopImpl* loop);
  ~TaskRunnerImpl();
public:
  /*
  @func			: PostTask
  @brief		: 投递任务。
  */
  virtual void PostTask(const Closure& task);
  /*
  @func			: Wakeup
  @brief		: 呼醒Loop。
  */
  virtual void Wakeup();
  /*
  @func			: RunTasks
  @brief		: 执行任务。
  */
	void RunTasks();
  /*
  @func			: WillDestroy
  @brief		: 
  */
  void WillDestroy();
protected:
	static void wakeup_cb(uv_async_t* handle);
private:
	uv_async_t                          wakeup_req_;      //呼醒请求
  yx::QueueT<Closure>									task_queue_;      //任务队列
  yx::QueueT<Closure>                 work_queue_;      //工作队列
  yx::SpinLock                        lock_;
};

}; // namespace yx



// -------------------------------------------------------------------------
#endif /* TASK_RUNNER_IMPL_H_ */
