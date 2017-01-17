/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\loop_worker.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-1 17:15
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef LOOP_RUNNER_H_
#define LOOP_RUNNER_H_
#include <atomic>
#include "yx_export.h"
#include "task_runner.h"
// -------------------------------------------------------------------------
namespace yx {
class Loop;
//////////////////////////////////////////////////////////////////////////
// LoopRunner
class YX_EXPORT LoopRunner
{
public:
  LoopRunner();
  ~LoopRunner();
  enum class EStatus {
    Start = 0,
    Running,
    Shutdown,
    Stop,
  };
public:
  /*
  @func			: Start
  @brief		: 
  */
  void Start();
  /*
  @func			: Stop
  @brief		: 
  */
  void Stop();
  /*
  @func			: status
  @brief		: 
  */
  EStatus status() {
    return status_;
  }
  /*
  @func			: PostTask
  @brief		:
  */
  void PostTask(const Closure& task);
  /*
  @func			: task_runner
  @brief		:
  */
  std::shared_ptr<TaskRunner> task_runner() {
    return task_runner_;
  }
  uint8_t loop_id() const { 
    return loop_id_;
  }
protected:
  static void thread_run(void* arg);
  /*
  @func			: OnRun
  @brief		: 
  */
  virtual void OnRun(Loop* loop);
private:
  uint8_t loop_id_;
  char thread_storage_[32];
  std::atomic<EStatus> status_;
  std::shared_ptr<TaskRunner> task_runner_;
};

}; // namespace yx

// -------------------------------------------------------------------------
#endif /* LOOP_RUNNER_H_ */
