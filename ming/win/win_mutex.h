#ifndef MING_WIN_MUTEX_H_
#define MING_WIN_MUTEX_H_

#include <windows.h>
#include "ming/noncopyable.h"
#include "ming/scoped_lock.h"

namespace ming {

class Mutex : private noncopyable {
 public:
  typedef ming::ScopedLock<Mutex> ScopedLock;

  Mutex() { init(); }

  ~Mutex() { ::DeleteCriticalSection(&crit_section_); }

  void Lock() { ::EnterCriticalSection(&crit_section_); }

  void Unlock() { ::LeaveCriticalSection(&crit_section_); }

 private:
  int init() {
    // if the critical section is unavailable, the calling thread spins
    // dwSpinCount times before performing a wait operation on a semaphore
    // associated with the critical section. If the critical section becomes
    //  free during the spin operation, the calling thread avoids the
    //   wait operation.
    if (!::InitializeCriticalSectionAndSpinCount(&crit_section_, 4096)) {
      return ::GetLastError();
    } else {
      return 0;
    }
  }

  ::CRITICAL_SECTION crit_section_;
};

}  // namespace ming

#endif  // MING_WIN_MUTEX_H_
