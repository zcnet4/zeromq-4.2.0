/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\loop.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-3 17:55
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef LOOP_H_
#define LOOP_H_
#include <memory>
#include "yx_export.h"
#include "task_runner.h"
// -------------------------------------------------------------------------
struct uv_loop_s;
typedef uv_loop_s uv_loop_t;
namespace yx {
//////////////////////////////////////////////////////////////////////////
//
class Tcp;
class LoopImpl;
class SocketManager;
//////////////////////////////////////////////////////////////////////////
//
class YX_EXPORT Loop {
public:
  Loop();
  ~Loop();
  static Loop* current();
public:
	/*
	@func			: Run
	@brief		:
	*/
	int Run();
	/*
	@func			: Quit
	@brief		:
	*/
	void Quit();
  /*
  @func			: PostTask
  @brief		: 
  */
  void PostTask(const Closure& task);
  /*
  @func			: Watch
  @brief		: 
  */
  void Watch(const Closure& task);
	/*
	@func			: task_runner
	@brief		:
	*/
	std::shared_ptr<TaskRunner> task_runner();
	/*
	@func			: loop_impl
	@brief		:
	*/
	LoopImpl* loop_impl();
  /*
  @func			: loop_id
  @brief		:
  */
  uint8_t loop_id() const;
  /*
  @func			: uv_loop
  @brief		: 
  */
  uv_loop_t* uv_loop();
  /*
  @func			: ticks_now
  @brief		: 
  */
  uint64_t ticks_now() const;
  /*
  @func			: time_now
  @brief		: 
  */
  uint64_t time_now() const;
  /*
  @func			: socket_manager
  @brief		: 
  */
  SocketManager* socket_manager();
private:
	std::unique_ptr<LoopImpl> impl_;
};

/*
@func			: SetMachineId
@brief		: 
*/
YX_EXPORT void SetMachineId(uint8_t machine_id);
/*
@func			: GetMachineId
@brief		: 
*/
YX_EXPORT uint8_t GetMachineId();

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* LOOP_H_ */
