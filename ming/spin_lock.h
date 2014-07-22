#ifndef MING_SPIN_LOCK_H_
#define MING_SPIN_LOCK_H_

#if defined(_MSC_VER)
  // Microsoft Visual C++
  #include "ming/win/win_mutex.h"
namespace ming {
  typedef ming::Mutex SpinLock;
}
#elif defined(__GNUC__)
  // GNU C++
  #include "ming/linux/linux_spin_lock.h"
#else
  #error "Support Windows and Linux platform Only!"
#endif

#endif // MING_SPIN_LOCK_H_
