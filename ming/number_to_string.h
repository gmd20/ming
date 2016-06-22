#ifndef MING_NUMBER_TO_STRING_H_
#define MING_NUMBER_TO_STRING_H_

#include <stdint.h>
#include "ming/folly/Likely.h"

// --------------------------------------------------------------
// 说明：
// 1. u32toa_branchlut 性能好于 uint64ToBufferUnsafe
// 2. 使用但一个内存块重复测试时， format_int 在数字大的时候快于
//    uint64ToBufferUnsafe。  在数字小时uint64ToBufferUnsafe 比
//    format_int稍微快一点点
// 3. 在32位系统还是需要单独定义一个uint32_t的uint64ToBufferUnsafe版本
// --------------------------------------------------------------

namespace ming {

int32_t u32toa(uint32_t value, char* buffer);
int32_t i32toa(int32_t value, char* buffer);
int32_t u64toa(uint64_t value, char* buffer);
int32_t i64toa(int64_t value, char* buffer);
int32_t dtoa(double value, char* buffer);

//-----------------------------------------------------------------------------
// https://github.com/facebook/folly/blob/master/folly/Conv.h
//-----------------------------------------------------------------------------
/*
 * Copyright 2015 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Returns the number of digits in the base 10 representation of an
 * uint64_t. Useful for preallocating buffers and such. It's also used
 * internally, see below. Measurements suggest that defining a
 * separate overload for 32-bit integers is not worthwhile.
 */

inline uint32_t digits10(uint64_t v) {
  uint32_t result = 1;
  for (;;) {
    if (LIKELY(v < 10)) return result;
    if (LIKELY(v < 100)) return result + 1;
    if (LIKELY(v < 1000)) return result + 2;
    if (LIKELY(v < 10000)) return result + 3;
    // Skip ahead by 4 orders of magnitude
    v /= 10000U;
    result += 4;
  }
}

/**
 * Copies the ASCII base 10 representation of v into buffer and
 * returns the number of bytes written. Does NOT append a \0. Assumes
 * the buffer points to digits10(v) bytes of valid memory. Note that
 * uint64 needs at most 20 bytes, uint32_t needs at most 10 bytes,
 * uint16_t needs at most 5 bytes, and so on. Measurements suggest
 * that defining a separate overload for 32-bit integers is not
 * worthwhile.
 *
 * This primitive is unsafe because it makes the size assumption and
 * because it does not add a terminating \0.
 */

inline uint32_t uint64ToBufferUnsafe(uint64_t v, char* const buffer) {
  auto const result = digits10(v);
  // WARNING: using size_t or pointer arithmetic for pos slows down
  // the loop below 20x. This is because several 32-bit ops can be
  // done in parallel, but only fewer 64-bit ones.
  uint32_t pos = result - 1;
  while (v >= 10) {
    // Keep these together so a peephole optimization "sees" them and
    // computes them in one shot.
    auto const q = v / 10;
    auto const r = static_cast<uint32_t>(v % 10);
    buffer[pos--] = '0' + r;
    v = q;
  }
  // Last digit is trivial to handle
  buffer[pos] = static_cast<uint32_t>(v) + '0';
  return result;
}

//  使用的例子

// /**
//  * int32_t and int64_t to string (by appending) go through here. The
//  * result is APPENDED to a preexisting string passed as the second
//  * parameter. This should be efficient with fbstring because fbstring
//  * incurs no dynamic allocation below 23 bytes and no number has more
//  * than 22 bytes in its textual representation (20 for digits, one for
//  * sign, one for the terminating 0).
//  */
// template <class Tgt, class Src>
// typename std::enable_if<
//   std::is_integral<Src>::value && std::is_signed<Src>::value &&
//   IsSomeString<Tgt>::value && sizeof(Src) >= 4>::type
// toAppend(Src value, Tgt * result) {
//   char buffer[20];
//   if (value < 0) {
//     result->push_back('-');
//     result->append(buffer, uint64ToBufferUnsafe(-uint64_t(value), buffer));
//   } else {
//     result->append(buffer, uint64ToBufferUnsafe(value, buffer));
//   }
// }

// template <class Src>
// typename std::enable_if<
//   std::is_integral<Src>::value && std::is_signed<Src>::value
//   && sizeof(Src) >= 4 && sizeof(Src) < 16,
//   size_t>::type
// estimateSpaceNeeded(Src value) {
//   if (value < 0) {
//     return 1 + digits10(static_cast<uint64_t>(-value));
//   }

//   return digits10(static_cast<uint64_t>(value));
// }

// /**
//  * As above, but for uint32_t and uint64_t.
//  */
// template <class Tgt, class Src>
// typename std::enable_if<
//   std::is_integral<Src>::value && !std::is_signed<Src>::value
//   && IsSomeString<Tgt>::value && sizeof(Src) >= 4>::type
// toAppend(Src value, Tgt * result) {
//   char buffer[20];
//   result->append(buffer, buffer + uint64ToBufferUnsafe(value, buffer));
// }

// template <class Src>
// typename std::enable_if<
//   std::is_integral<Src>::value && !std::is_signed<Src>::value
//   && sizeof(Src) >= 4 && sizeof(Src) < 16,
//   size_t>::type
// estimateSpaceNeeded(Src value) {
//   return digits10(value);
// }

// /*
//  * Lookup tables that converts from a decimal character value to an integral
//  * binary value, shifted by a decimal "shift" multiplier.
//  * For all character values in the range '0'..'9', the table at those
//  * index locations returns the actual decimal value shifted by the
//  multiplier.
//  * For all other values, the lookup table returns an invalid OOR value.
//  */
// // Out-of-range flag value, larger than the largest value that can fit in
// // four decimal bytes (9999), but four of these added up together should
// // still not overflow uint16_t.
// constexpr int32_t OOR = 10000;

// __attribute__((__aligned__(16))) constexpr uint16_t shift1[] = {
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 0-9
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  10
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  20
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  30
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, 0,         //  40
//   1, 2, 3, 4, 5, 6, 7, 8, 9, OOR, OOR,
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  60
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  70
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  80
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  90
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 100
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 110
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 120
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 130
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 140
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 150
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 160
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 170
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 180
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 190
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 200
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 210
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 220
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 230
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 240
//   OOR, OOR, OOR, OOR, OOR, OOR                       // 250
// };

// __attribute__((__aligned__(16))) constexpr uint16_t shift10[] = {
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 0-9
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  10
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  20
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  30
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, 0,         //  40
//   10, 20, 30, 40, 50, 60, 70, 80, 90, OOR, OOR,
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  60
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  70
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  80
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  90
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 100
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 110
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 120
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 130
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 140
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 150
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 160
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 170
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 180
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 190
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 200
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 210
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 220
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 230
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 240
//   OOR, OOR, OOR, OOR, OOR, OOR                       // 250
// };

// __attribute__((__aligned__(16))) constexpr uint16_t shift100[] = {
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 0-9
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  10
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  20
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  30
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, 0,         //  40
//   100, 200, 300, 400, 500, 600, 700, 800, 900, OOR, OOR,
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  60
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  70
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  80
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  90
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 100
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 110
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 120
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 130
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 140
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 150
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 160
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 170
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 180
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 190
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 200
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 210
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 220
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 230
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 240
//   OOR, OOR, OOR, OOR, OOR, OOR                       // 250
// };

// __attribute__((__aligned__(16))) constexpr uint16_t shift1000[] = {
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 0-9
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  10
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  20
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  30
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, 0,         //  40
//   1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, OOR, OOR,
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  60
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  70
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  80
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  //  90
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 100
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 110
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 120
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 130
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 140
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 150
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 160
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 170
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 180
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 190
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 200
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 210
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 220
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 230
//   OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR, OOR,  // 240
//   OOR, OOR, OOR, OOR, OOR, OOR                       // 250
// };

// /**
//  * String represented as a pair of pointers to char to unsigned
//  * integrals. Assumes NO whitespace before or after, and also that the
//  * string is composed entirely of digits. Tgt must be unsigned, and no
//  * sign is allowed in the string (even it's '+'). String may be empty,
//  * in which case digits_to throws.
//  */
//   template <class Tgt>
//   Tgt digits_to(const char * b, const char * e) {

//     static_assert(!std::is_signed<Tgt>::value, "Unsigned type expected");
//     assert(b <= e);

//     const size_t size = e - b;

//     /* Although the string is entirely made of digits, we still need to
//      * check for overflow.
//      */
//     if (size >= std::numeric_limits<Tgt>::digits10 + 1) {
//       // Leading zeros? If so, recurse to keep things simple
//       if (b < e && *b == '0') {
//         for (++b;; ++b) {
//           if (b == e) return 0; // just zeros, e.g. "0000"
//           if (*b != '0') return digits_to<Tgt>(b, e);
//         }
//       }
//       FOLLY_RANGE_CHECK_BEGIN_END(
//         size == std::numeric_limits<Tgt>::digits10 + 1 &&
//         strncmp(b, detail::MaxString<Tgt>::value, size) <= 0,
//         "Numeric overflow upon conversion", b, e);
//     }

//     // Here we know that the number won't overflow when
//     // converted. Proceed without checks.

//     Tgt result = 0;

//     for (; e - b >= 4; b += 4) {
//       result *= 10000;
//       const int32_t r0 = shift1000[static_cast<size_t>(b[0])];
//       const int32_t r1 = shift100[static_cast<size_t>(b[1])];
//       const int32_t r2 = shift10[static_cast<size_t>(b[2])];
//       const int32_t r3 = shift1[static_cast<size_t>(b[3])];
//       const auto sum = r0 + r1 + r2 + r3;
//       assert(sum < OOR && "Assumption: string only has digits");
//       result += sum;
//     }

//     switch (e - b) {
//       case 3: {
//         const int32_t r0 = shift100[static_cast<size_t>(b[0])];
//         const int32_t r1 = shift10[static_cast<size_t>(b[1])];
//         const int32_t r2 = shift1[static_cast<size_t>(b[2])];
//         const auto sum = r0 + r1 + r2;
//         assert(sum < OOR && "Assumption: string only has digits");
//         return result * 1000 + sum;
//       }
//       case 2: {
//         const int32_t r0 = shift10[static_cast<size_t>(b[0])];
//         const int32_t r1 = shift1[static_cast<size_t>(b[1])];
//         const auto sum = r0 + r1;
//         assert(sum < OOR && "Assumption: string only has digits");
//         return result * 100 + sum;
//       }
//       case 1: {
//         const int32_t sum = shift1[static_cast<size_t>(b[0])];
//         assert(sum < OOR && "Assumption: string only has digits");
//         return result * 10 + sum;
//       }
//     }

//     assert(b == e);
//     FOLLY_RANGE_CHECK_BEGIN_END(size > 0,
//                                 "Found no digits to convert in input", b, e);
//     return result;
//   }

}  // namespace ming

#endif  // MING_NUMBER_TO_STRING_H_
