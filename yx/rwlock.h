/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\rwlock.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-2 17:14
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef RWLOCK_H_
#define RWLOCK_H_
#include "atomic.h"
// -------------------------------------------------------------------------

#ifndef USE_PTHREAD_LOCK

struct rwlock {
  int write;
  int read;
};

static inline void
rwlock_init(struct rwlock *lock) {
  lock->write = 0;
  lock->read = 0;
}

static inline void
rwlock_rlock(struct rwlock *lock) {
  for (;;) {
    while (lock->write) {
      __sync_synchronize();
    }
    __sync_add_and_fetch(&lock->read, 1);
    if (lock->write) {
      __sync_sub_and_fetch(&lock->read, 1);
    } else {
      break;
    }
  }
}

static inline void
rwlock_wlock(struct rwlock *lock) {
  while (__sync_lock_test_and_set(&lock->write, 1)) {}
  while (lock->read) {
    __sync_synchronize();
  }
}

static inline void
rwlock_wunlock(struct rwlock *lock) {
  __sync_lock_release(&lock->write);
}

static inline void
rwlock_runlock(struct rwlock *lock) {
  __sync_sub_and_fetch(&lock->read, 1);
}

#else

#include <pthread.h>

// only for some platform doesn't have __sync_*
// todo: check the result of pthread api

struct rwlock {
  pthread_rwlock_t lock;
};

static inline void
rwlock_init(struct rwlock *lock) {
  pthread_rwlock_init(&lock->lock, NULL);
}

static inline void
rwlock_rlock(struct rwlock *lock) {
  pthread_rwlock_rdlock(&lock->lock);
}

static inline void
rwlock_wlock(struct rwlock *lock) {
  pthread_rwlock_wrlock(&lock->lock);
}

static inline void
rwlock_wunlock(struct rwlock *lock) {
  pthread_rwlock_unlock(&lock->lock);
}

static inline void
rwlock_runlock(struct rwlock *lock) {
  pthread_rwlock_unlock(&lock->lock);
}

#endif

// -------------------------------------------------------------------------
#endif /* RWLOCK_H_ */
