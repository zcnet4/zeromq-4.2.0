/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\task_runner.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-3 18:30
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef TASK_RUNNER_H_
#define TASK_RUNNER_H_
#include <memory>
#include <functional>
// -------------------------------------------------------------------------
namespace yx {

typedef std::function<void(void)> Closure;

//////////////////////////////////////////////////////////////////////////
//
class TaskRunner
{
public:
  uint8_t loop_id() const { return loop_id_; }
	/*
	@func			: PostTask
	@brief		: 投递任务。
	*/
	virtual void PostTask(const Closure& task) = 0;
  /*
  @func			: Wakeup
  @brief		: 呼醒Loop。
  */
  virtual void Wakeup() = 0;
protected:
  uint8_t                            loop_id_;
};

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* TASK_RUNNER_H_ */
