#ifndef MING_LINUX_MUTEX_H_
#define MING_LINUX_MUTEX_H_

#include <pthread.h>
#include "ming/noncopyable.h"
#include "ming/scoped_lock.h"

namespace ming {

class Mutex : private noncopyable
{
public:
  typedef ming::ScopedLock<Mutex> ScopedLock;

  Mutex()
  {
    ::pthread_mutex_init(&mutex_, 0);
  }

  ~Mutex()
  {
    ::pthread_mutex_destroy(&mutex_); // Ignore EBUSY.
  }

  void Lock()
  {
    (void)::pthread_mutex_lock(&mutex_); // Ignore EINVAL.
  }

  void Unlock()
  {
    (void)::pthread_mutex_unlock(&mutex_); // Ignore EINVAL.
  }

private:
  ::pthread_mutex_t mutex_;
};

} // namespace ming

#endif // MING_LINUX_MUTEX_H_
