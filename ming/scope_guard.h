#ifndef MING_SCOPE_GUARD_H_
#define MING_SCOPE_GUARD_H_

#if _MSC_VER <= 1700
// Visual Studio 2013 doesn't support the c++ 11 noexcept keyword yest
#include <boost/scope_exit.hpp>
#endif

#ifdef __GNUC__
#include <folly/ScopeGuard.h>
#endif

#endif  // MING_SCOPE_GUARD_H_

