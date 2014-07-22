#ifndef MING_LINUX_SPIN_LOCK_H_
#define MING_LINUX_SPIN_LOCK_H_

#include <pthread.h>
#include "ming/noncopyable.h"
#include "ming/scoped_lock.h"

namespace ming {

class SpinLock : private noncopyable
{
public:
  typedef ming::ScopedLock<SpinLock> ScopedLock;

  SpinLock()
  {
    ::pthread_spin_init(&spin_lock_, 0);
  }

  ~SpinLock()
  {
    ::pthread_spin_destroy(&spin_lock_);
  }

  void Lock()
  {
    (void)::pthread_spin_lock(&spin_lock_);
  }

  void Unlock()
  {
    (void)::pthread_spin_unlock(&spin_lock_);
  }

private:
  ::pthread_spinlock_t spin_lock_;
};

} // namespace ming

#endif // MING_LINUX_SPIN_LOCK_H_
