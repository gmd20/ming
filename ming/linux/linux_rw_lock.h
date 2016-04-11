#ifndef MING_LINUX_RW_LOCK_H_
#define MING_LINUX_RW_LOCK_H_

#include <pthread.h>
#include "ming/noncopyable.h"
#include "ming/scoped_lock.h"

namespace ming {

class RWLock : private noncopyable {
 public:
  RWLock() { ::pthread_rwlock_init(&rw_lock_, 0); }

  ~RWLock() { ::pthread_rwlock_destroy(&rw_lock_); }

  void ReadLock() { (void)::pthread_rwlock_rdlock(&rw_lock_); }

  void WriteLock() { (void)::pthread_rwlock_wrlock(&rw_lock_); }

  void Unlock() { (void)::pthread_rwlock_unlock(&rw_lock_); }

  class ReadScopedLock {
   public:
    explicit ReadScopedLock(RWLock& lock) : lock_(lock) { lock_.ReadLock(); }
    ~ReadScopedLock() { lock_.Unlock(); }

   private:
    RWLock& lock_;
  };

  class WriteScopedLock {
   public:
    explicit WriteScopedLock(RWLock& lock) : lock_(lock) { lock_.WriteLock(); }
    ~WriteScopedLock() { lock_.Unlock(); }

   private:
    RWLock& lock_;
  };

 private:
  ::pthread_rwlock_t rw_lock_;
};

}  // namespace ming

#endif  // MING_LINUX_RW_LOCK_H_
