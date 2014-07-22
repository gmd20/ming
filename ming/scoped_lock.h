#ifndef MING_SCOPED_LOCK_H_
#define MING_SCOPED_LOCK_H_

#include "ming/noncopyable.h"

namespace ming {

template <typename LockType>
class ScopedLock : private noncopyable
{
public:
  explicit ScopedLock(LockType& lock)
    : lock_(lock)
  {
    lock_.Lock();
  }

  ~ScopedLock()
  {
    lock_.Unlock();
  }

private:
   LockType&   lock_;
};

}  // namespace ming

#endif // MING_SCOPED_LOCK_H_
