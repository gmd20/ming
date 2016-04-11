#ifndef MING_NONCOPYABLE_H_
#define MING_NONCOPYABLE_H_

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
//  murmurhash3   32bit version
//  from https://github.com/dcjones/hat-trie/blob/master/src/murmurhash3.c
//========================================================================

/* This is MurmurHash3. The original C++ code was placed in the public domain
 * by its author, Austin Appleby. */

inline uint32_t fmix(uint32_t h) {
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

inline uint32_t rotl32(uint32_t x, int8_t r) {
  return (x << r) | (x >> (32 - r));
}

uint32_t murmurhash3(const char* data, int len_) {
  const int len = (int)len_;
  const int nblocks = len / 4;

  uint32_t h1 = 0xc062fb4a;

  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);

  int i;
  for (i = -nblocks; i; i++) {
    uint32_t k1 = blocks[i];

    k1 *= c1;
    k1 = rotl32(k1, 15);
    k1 *= c2;

    h1 ^= k1;
    h1 = rotl32(h1, 13);
    h1 = h1 * 5 + 0xe6546b64;
  }

  //----------
  // tail

  const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);

  uint32_t k1 = 0;

  switch (len & 3) {
    case 3:
      k1 ^= tail[2] << 16;
    case 2:
      k1 ^= tail[1] << 8;
    case 1:
      k1 ^= tail[0];
      k1 *= c1;
      k1 = rotl32(k1, 15);
      k1 *= c2;
      h1 ^= k1;
  }

  //----------
  // finalization

  h1 ^= len;

  h1 = fmix(h1);

  return h1;
}

//========================================================================
//  murmurhash  64 bit version
//  from
//  https://github.com/BaiduPS/sofa-pbrpc/blob/master/src/sofa/pbrpc/murmurhash.h
//========================================================================

// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

inline uint64_t murmurhash(const void* key, int len) {
  const uint64_t m = 0xc6a4a7935bd1e995;
  const int r = 47;
  uint64_t h = 0x5bd1e995;

  const uint64_t* data = (const uint64_t*)key;
  const uint64_t* end = data + (len / 8);

  while (data != end) {
    uint64_t k = *data++;

    k *= m;
    k ^= k >> r;
    k *= m;

    h ^= k;
    h *= m;
  }

  const unsigned char* data2 = (const unsigned char*)data;

  switch (len & 7) {
    case 7:
      h ^= uint64_t(data2[6]) << 48;
    case 6:
      h ^= uint64_t(data2[5]) << 40;
    case 5:
      h ^= uint64_t(data2[4]) << 32;
    case 4:
      h ^= uint64_t(data2[3]) << 24;
    case 3:
      h ^= uint64_t(data2[2]) << 16;
    case 2:
      h ^= uint64_t(data2[1]) << 8;
    case 1:
      h ^= uint64_t(data2[0]);
      h *= m;
      break;
    default:
      break;
  };

  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
}

// =======================================
// farmhash
// https://code.google.com/p/farmhash/source/browse/trunk/src/farmhash.cc
// =======================================

}  // namespace hash
}  // namespace ming

#endif  // MING_NONCOPYABLE_H_
