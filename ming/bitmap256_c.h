// Copyright 2016 The RE2 Authors.  All Rights Reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef RE2_BITMAP256_H_
#define RE2_BITMAP256_H_

#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <stdint.h>
#include <string.h>
#include <assert.h>

// Debug-only checking.
#define DCHECK(condition) assert(condition)
#define DCHECK_EQ(val1, val2) assert((val1) == (val2))
#define DCHECK_NE(val1, val2) assert((val1) != (val2))
#define DCHECK_LE(val1, val2) assert((val1) <= (val2))
#define DCHECK_LT(val1, val2) assert((val1) < (val2))
#define DCHECK_GE(val1, val2) assert((val1) >= (val2))
#define DCHECK_GT(val1, val2) assert((val1) > (val2))

#define arraysize(array) (int)(sizeof(array)/sizeof((array)[0]))

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif


struct Bitmap256 {
  uint64_t words[4];
};

void bitmap256_init(struct Bitmap256 *bitmap) {
  memset(bitmap->words, 0, sizeof bitmap->words);
}
#define bitmap256_clear(b) bitmap256_init(b)

// Tests the bit with index c.
int bitmap256_test(struct Bitmap256 *bitmap, int c) {
  DCHECK_GE(c, 0);
  DCHECK_LE(c, 255);

  return (bitmap->words[c / 64] & (1ULL << (c % 64))) != 0;
}

  // Sets the bit with index c.
void bitmap256_set(struct Bitmap256 *bitmap, int c) {
  DCHECK_GE(c, 0);
  DCHECK_LE(c, 255);

  bitmap->words[c / 64] |= (1ULL << (c % 64));
}


// Finds the least significant non-zero bit in n.
static int FindLSBSet(uint64_t n) {
  DCHECK_NE(n, 0);

#if defined(__GNUC__)
  return __builtin_ctzll(n);
#elif defined(_MSC_VER) && defined(_M_X64)
  unsigned long c;
  _BitScanForward64(&c, n);
  return static_cast<int>(c);
#elif defined(_MSC_VER) && defined(_M_IX86)
  unsigned long c;
  if (static_cast<uint32_t>(n) != 0) {
    _BitScanForward(&c, static_cast<uint32_t>(n));
    return static_cast<int>(c);
  } else {
    _BitScanForward(&c, static_cast<uint32_t>(n >> 32));
    return static_cast<int>(c) + 32;
  }
#else
  int c = 63;
  for (int shift = 1 << 5; shift != 0; shift >>= 1) {
    uint64_t word = n << shift;
    if (word != 0) {
      n = word;
      c -= shift;
    }
  }
  return c;
#endif
}


// Finds the next non-zero bit with index >= c.
// Returns -1 if no such bit exists.
int bitmap256_findnextsetbit(struct Bitmap256 *bitmap, int c) {
  DCHECK_GE(c, 0);
  DCHECK_LE(c, 255);

  // Check the word that contains the bit. Mask out any lower bits.
  int i = c / 64;
  uint64_t word = bitmap->words[i] & (~0ULL << (c % 64));
  if (word != 0)
    return (i * 64) + FindLSBSet(word);

  // Check any following words.
  i++;
  switch (i) {
    case 1:
      if (bitmap->words[1] != 0)
        return (1 * 64) + FindLSBSet(bitmap->words[1]);
      FALLTHROUGH_INTENDED;
    case 2:
      if (bitmap->words[2] != 0)
        return (2 * 64) + FindLSBSet(bitmap->words[2]);
      FALLTHROUGH_INTENDED;
    case 3:
      if (bitmap->words[3] != 0)
        return (3 * 64) + FindLSBSet(bitmap->words[3]);
      FALLTHROUGH_INTENDED;
    default:
      return -1;
  }
}


#endif  // RE2_BITMAP256_H_
