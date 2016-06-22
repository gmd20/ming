#ifndef MING_NONCOPYABLE_H_
#define MING_NONCOPYABLE_H_

#include <stdint.h>
#include <string>


namespace ming {
namespace hash {

//==================================================================
// fnv
// from https://github.com/facebook/folly/blob/master/folly/Hash.h
//==================================================================

/*
 * Fowler / Noll / Vo (FNV) Hash
 *     http://www.isthe.com/chongo/tech/comp/fnv/
 */

const uint32_t FNV_32_HASH_START = 2166136261UL;
const uint64_t FNV_64_HASH_START = 14695981039346656037ULL;

inline uint32_t fnv32(const char* s, uint32_t hash = FNV_32_HASH_START) {
  for (; *s; ++s) {
    hash += (hash << 1) + (hash << 4) + (hash << 7) + (hash << 8) + (hash << 24);
    hash ^= *s;
  }
  return hash;
}

inline uint32_t fnv32_buf(const void* buf, int n,
                          uint32_t hash = FNV_32_HASH_START) {
  const char* char_buf = reinterpret_cast<const char*>(buf);

  for (int i = 0; i < n; ++i) {
    hash += (hash << 1) + (hash << 4) + (hash << 7) + (hash << 8) + (hash << 24);
    hash ^= char_buf[i];
  }

  return hash;
}

inline uint32_t fnv32(const std::string& str,
                      uint32_t hash = FNV_32_HASH_START) {
  return fnv32_buf(str.data(), str.size(), hash);
}

inline uint64_t fnv64(const char* s, uint64_t hash = FNV_64_HASH_START) {
  for (; *s; ++s) {
    hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) +
            (hash << 8) + (hash << 40);
    hash ^= *s;
  }
  return hash;
}

inline uint64_t fnv64_buf(const void* buf, int n,
                          uint64_t hash = FNV_64_HASH_START) {
  const char* char_buf = reinterpret_cast<const char*>(buf);

  for (int i = 0; i < n; ++i) {
    hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) +
            (hash << 8) + (hash << 40);
    hash ^= char_buf[i];
  }
  return hash;
}

inline uint64_t fnv64(const std::string& str,
                      uint64_t hash = FNV_64_HASH_START) {
  return fnv64_buf(str.data(), str.size(), hash);
}

//========================================================================
//  murmurhash3
// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash1.h
// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.
//========================================================================

uint32_t murmurhash1(const void* key, int len, uint32_t seed);
unsigned int murmurhash1_aligned(const void* key, int len, unsigned int seed);
void murmurhash3_x86_32(const void* key, int len, uint32_t seed, void* out);
void murmurhash3_x86_128(const void* key, const int len, uint32_t seed,
                         void* out);
void murmurhash3_x64_128(const void* key, const int len, const uint32_t seed,
                         void* out);

// =======================================
// farmhash
// https://code.google.com/p/farmhash/source/browse/trunk/src/farmhash.cc
// =======================================

}  // namespace hash
}  // namespace ming

#endif  // MING_NONCOPYABLE_H_
