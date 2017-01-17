/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\runner.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-4 12:03
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef RUNNER_H_
#define RUNNER_H_
#include "yx/loop.h"
#include "yx/loop_runner.h"
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// Runner
class Runner
{
public:
  enum ID {
    MAIN = 0,
    WORK,
    FRONTEND,
    BACKEND,
    ID_COUNT,
  };
  Runner();
  ~Runner();
public:
  /*
  @func			: Start
  @brief		: 
  */
  void Start();
  /*
  @func			: Run
  @brief		: 
  */
  void Run();
  /*
  @func			: Stop
  @brief		: 
  */
  void Stop();
  /*
  @func			: PostTask
  @brief		: 
  */
  void PostTask(ID id, const yx::Closure& task);
  /*
  @func			: PostTask
  @brief		:
  */
  void PostTask(uint16_t loop_id, const yx::Closure& task);
  /*
  @func			: CloseTcp
  @brief		: 
  */
  void CloseTcp(uint64_t tcp_id);
  /*
  @func			: main_runner
  @brief		: 
  */
  std::shared_ptr<yx::TaskRunner> main_runner();
  /*
  @func			: now_mainLoop
  @brief		: 
  */
  uint64_t now_mainLoop() const;
protected:
  yx::Loop main_runner_;
  yx::LoopRunner runners_[ID_COUNT - 1];
};


// -------------------------------------------------------------------------
#endif /* RUNNER_H_ */
