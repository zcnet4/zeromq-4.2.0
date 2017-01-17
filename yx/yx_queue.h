/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\yx_queue.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-2 17:25
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef YX_QUEUE_H_
#define YX_QUEUE_H_
#include <memory>
#include "spinlock.h"
#include "noncopyable.h"
#include "allocator.h"
// -------------------------------------------------------------------------
namespace yx {
struct SpinLock {
  SpinLock() { spinlock_init(&lock_); }
  ~SpinLock() { spinlock_destroy(&lock_); }
  void lock() { spinlock_lock(&lock_); }
  void unlock() { spinlock_unlock(&lock_); }
  spinlock lock_;
};
struct NonLock {
  NonLock(){}
  ~NonLock(){}
  void lock(){}
  void unlock(){}
};
//////////////////////////////////////////////////////////////////////////
// QueueT
template<typename T, int N = 1024, typename LOCK = NonLock>
class QueueT : noncopyable {
public:
  QueueT() : read_index_(0), write_index_(0), max_size_(N) {
    queue_ = (T*)yx_malloc(sizeof(T) * max_size_);
  }
  ~QueueT(){
    yx_free(queue_);
  }
public:
  /*
  @func			: push
  @brief		: 
  */
  void push(const T& elem) {
    lock_.lock();
    //
    ::new (queue_ + write_index_) T(elem);
    if (++write_index_ >= max_size_) {
      write_index_ = 0;
    }

    if (read_index_ == write_index_) {
      T* new_queue = (T*)yx_malloc(sizeof(T) * max_size_ * 2);
      for (int i = 0; i < max_size_; i++) {
        T& ref_old = queue_[(read_index_ + i) % max_size_];
        ::new (new_queue + i) T(ref_old);
        ref_old.~T();
      }
      read_index_ = 0;
      write_index_ = max_size_;
      max_size_ *= 2;
      //
      yx_free(queue_);
      queue_ = new_queue;
    }
    //
    lock_.unlock();
  }
  /*
  @func			: pop
  @brief		: 
  */
  bool pop(T& elem) {
    bool success = false;

    lock_.lock();
    if (read_index_ != write_index_) {
      elem = std::move(queue_[read_index_]);
      queue_[read_index_].~T();
      if (++read_index_ >= max_size_)
        read_index_ = 0;
      success = true;
    }
    lock_.unlock();
    //
    return success;
  }
  /*
  @func			: size
  @brief		: 
  */
  int size() {
    int read_index, write_index, max_size;
    //
    lock_.lock();
    read_index = read_index_;
    write_index = write_index_;
    max_size = max_size_;
    lock_.unlock();
    //
    if (read_index <= write_index) {
      return write_index - read_index;
    }
    return write_index + max_size - read_index;
  }
  /*
  @func			: swap
  @brief		: 
  */
  void swap(QueueT<T, N, LOCK>& src) {
    lock_.lock();
    std::swap(max_size_, src.max_size_);
    std::swap(read_index_, src.read_index_);
    std::swap(write_index_, src.write_index_);
    std::swap(queue_, src.queue_);
    lock_.unlock();
  }
private:
  int read_index_;
  int write_index_;
  int max_size_;
  T* queue_;
  LOCK lock_;
};

//////////////////////////////////////////////////////////////////////////
// SpinQueueT
template<typename T, int N = 1000>
class SpinQueueT : public QueueT<T, N, SpinLock>
{

};

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* YX_QUEUE_H_ */
