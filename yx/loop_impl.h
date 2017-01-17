/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\loop_impl.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-26 15:38
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef LOOP_IMPL_H_
#define LOOP_IMPL_H_
#include <memory>
#include <stdint.h>
#include "uv.h"
#include "task_runner.h"
#include "memory_pool.h"
#include "id_allocator.h"
#include "socket_manager.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
//
class Loop;
class TaskRunnerImpl;
//////////////////////////////////////////////////////////////////////////
// LoopImpl
class LoopImpl
{
public:
  LoopImpl(Loop* loop);
  ~LoopImpl();
public:
  uv_loop_t* uv_loop() { return &uv_loop_; }
  Loop* loop() { return loop_; }
  uint8_t loop_id() const { return fd_allocator_.worker_id(); }
  MemoryPool* recv_cache_pool() { return &recv_cache_pool_; }
  SocketManager* socket_manager() { return &socket_manager_; }
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
  @func			: Wakeup
  @brief		: 
  */
  void Wakeup();
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
  @func			: Register
  @brief		:
  */
  uint64_t Register(Tcp* tcp);
  uint64_t Register(Udp* udp);
  /*
  @func			: Unregister
  @brief		:
  */
  void Unregister(uint64_t id);
	/*
	@func			: task_runner
	@brief		:
	*/
	std::shared_ptr<TaskRunner> task_runner();
  /*
  @func			: time_now
  @brief		: 
  */
  uint64_t time_now();
  /*
  @func			: alloc_fd
  @brief		: 分配文件描述符。
  */
  uint64_t alloc_fd();
  /*
  @func			: alloc_uv_req
  @brief		: 返回512B。
  */
  void* alloc_uv_req();
  /*
  @func			: free_uv_req
  @brief		: 
  */
  static void free_uv_req(void* p);
protected:
  static void task_cb(uv_prepare_t* handle);
  static void queue_cb(uv_prepare_t* handle);
private:
  Loop*                               loop_;
  uv_loop_t                           uv_loop_;
  uv_prepare_s                        task_watcher_;         //任务观察者
  uv_prepare_s                        queue_watcher_;        //队列观察者
  Closure                             queue_task_;
  //http://locklessinc.com/的数据得出　lockless malloc比tcmalloc略优。
  MemoryPool                          recv_cache_pool_;        // 接收缓冲区内存池，1K大小固定内存池。
  MemoryPool                          uv_req_pool_;            // uv req缓冲区。
	std::shared_ptr<TaskRunnerImpl>			task_runner_;
  //减少锁竞争，我们希望fd依附Loop。
  IDAllocator                         fd_allocator_;
  SocketManager                       socket_manager_;
  // time_now
  uint64_t                            time_now_;
  uint64_t                            ticks_point_;
};

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* LOOP_IMPL_H_ */
