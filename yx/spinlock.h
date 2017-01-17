/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\spinlock.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-2 17:14
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef SPINLOCK_H_
#define SPINLOCK_H_
#include "atomic.h"
// ���������Ч�ʣ�����CPU����������Ԥ���¸������CPU��Ҳ�Ǽ�������������
// ticket lockЧ����ߣ��п���Ľ���
//http://locklessinc.com/articles/locks/
// -------------------------------------------------------------------------

#ifndef USE_PTHREAD_LOCK

struct spinlock {
  int lock;
};

static inline void
spinlock_init(struct spinlock *lock) {
  lock->lock = 0;
}

static inline void
spinlock_lock(struct spinlock *lock) {
  while (__sync_lock_test_and_set(&lock->lock, 1)) {}
}

static inline int
spinlock_trylock(struct spinlock *lock) {
  return __sync_lock_test_and_set(&lock->lock, 1) == 0;
}

static inline void
spinlock_unlock(struct spinlock *lock) {
  __sync_lock_release(&lock->lock);
}

static inline void
spinlock_destroy(struct spinlock *lock) {
  (void)lock;
}

#else

#include <pthread.h>

// we use mutex instead of spinlock for some reason
// you can also replace to pthread_spinlock

struct spinlock {
  pthread_mutex_t lock;
};

static inline void
spinlock_init(struct spinlock *lock) {
  pthread_mutex_init(&lock->lock, NULL);
}

static inline void
spinlock_lock(struct spinlock *lock) {
  pthread_mutex_lock(&lock->lock);
}

static inline int
spinlock_trylock(struct spinlock *lock) {
  return pthread_mutex_trylock(&lock->lock) == 0;
}

static inline void
spinlock_unlock(struct spinlock *lock) {
  pthread_mutex_unlock(&lock->lock);
}

static inline void
spinlock_destroy(struct spinlock *lock) {
  pthread_mutex_destroy(&lock->lock);
}

#endif

// -------------------------------------------------------------------------
#endif /* SPINLOCK_H_ */
