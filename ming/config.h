#ifndef MING_CONFIG_H_
#define MING_CONFIG_H_

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define OS_WINDOWS
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define OS_LINUX
#else
#error "Support Windows and Linux platform Only!"
#endif

#endif  // MING_CONFIG_H_
