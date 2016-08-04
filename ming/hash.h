#ifndef MING_NONCOPYABLE_H_
#define MING_NONCOPYABLE_H_

#include <stdint.h>
#include <stddef.h>
#include <string>

#ifdef _MSC_VER
# define INLINE __forceinline
#else
# define INLINE inline
#endif

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

// =======================================
// SpookyHash
// http://burtleburtle.net/bob/hash/spooky.html
// =======================================
//
// SpookyHash: a 128-bit noncryptographic hash function
// By Bob Jenkins, public domain
//   Oct 31 2010: alpha, framework + SpookyHash::Mix appears right
//   Oct 31 2011: alpha again, Mix only good to 2^^69 but rest appears right
//   Dec 31 2011: beta, improved Mix, tested it for 2-bit deltas
//   Feb  2 2012: production, same bits as beta
//   Feb  5 2012: adjusted definitions of uint* to be more portable
//   Mar 30 2012: 3 bytes/cycle, not 4.  Alpha was 4 but wasn't thorough enough.
//   August 5 2012: SpookyV2 (different results)
//
// Up to 3 bytes/cycle for long messages.  Reasonably fast for short messages.
// All 1 or 2 bit deltas achieve avalanche within 1% bias per output bit.
//
// This was developed for and tested on 64-bit x86-compatible processors.
// It assumes the processor is little-endian.  There is a macro
// controlling whether unaligned reads are allowed (by default they are).
// This should be an equally good hash on big-endian machines, but it will
// compute different results on them than on little-endian machines.
//
// Google's CityHash has similar specs to SpookyHash, and CityHash is faster
// on new Intel boxes.  MD4 and MD5 also have similar specs, but they are orders
// of magnitude slower.  CRCs are two or more times slower, but unlike
// SpookyHash, they have nice math for combining the CRCs of pieces to form
// the CRCs of wholes.  There are also cryptographic hashes, but those are even
// slower than MD5.
//

class SpookyHash
{
public:
    //
    // SpookyHash: hash a single message in one call, produce 128-bit output
    //
    static void Hash128(
        const void *message,  // message to hash
        size_t length,        // length of message in bytes
        uint64 *hash1,        // in/out: in seed 1, out hash value 1
        uint64 *hash2);       // in/out: in seed 2, out hash value 2

    //
    // Hash64: hash a single message in one call, return 64-bit output
    //
    static uint64 Hash64(
        const void *message,  // message to hash
        size_t length,        // length of message in bytes
        uint64 seed)          // seed
    {
        uint64 hash1 = seed;
        Hash128(message, length, &hash1, &seed);
        return hash1;
    }

    //
    // Hash32: hash a single message in one call, produce 32-bit output
    //
    static uint32 Hash32(
        const void *message,  // message to hash
        size_t length,        // length of message in bytes
        uint32 seed)          // seed
    {
        uint64 hash1 = seed, hash2 = seed;
        Hash128(message, length, &hash1, &hash2);
        return (uint32)hash1;
    }

    //
    // Init: initialize the context of a SpookyHash
    //
    void Init(
        uint64 seed1,       // any 64-bit value will do, including 0
        uint64 seed2);      // different seeds produce independent hashes

    //
    // Update: add a piece of a message to a SpookyHash state
    //
    void Update(
        const void *message,  // message fragment
        size_t length);       // length of message fragment in bytes

    //
    // Final: compute the hash for the current SpookyHash state
    //
    // This does not modify the state; you can keep updating it afterward
    //
    // The result is the same as if SpookyHash() had been called with
    // all the pieces concatenated into one message.
    //
    void Final(
        uint64 *hash1,    // out only: first 64 bits of hash value.
        uint64 *hash2);   // out only: second 64 bits of hash value.

    //
    // left rotate a 64-bit value by k bytes
    //
    static INLINE uint64 Rot64(uint64 x, int k)
    {
        return (x << k) | (x >> (64 - k));
    }

    //
    // This is used if the input is 96 bytes long or longer.
    //
    // The internal state is fully overwritten every 96 bytes.
    // Every input bit appears to cause at least 128 bits of entropy
    // before 96 other bytes are combined, when run forward or backward
    //   For every input bit,
    //   Two inputs differing in just that input bit
    //   Where "differ" means xor or subtraction
    //   And the base value is random
    //   When run forward or backwards one Mix
    // I tried 3 pairs of each; they all differed by at least 212 bits.
    //
    static INLINE void Mix(
        const uint64 *data,
        uint64 &s0, uint64 &s1, uint64 &s2, uint64 &s3,
        uint64 &s4, uint64 &s5, uint64 &s6, uint64 &s7,
        uint64 &s8, uint64 &s9, uint64 &s10,uint64 &s11)
    {
      s0 += data[0];    s2 ^= s10;    s11 ^= s0;    s0 = Rot64(s0,11);    s11 += s1;
      s1 += data[1];    s3 ^= s11;    s0 ^= s1;    s1 = Rot64(s1,32);    s0 += s2;
      s2 += data[2];    s4 ^= s0;    s1 ^= s2;    s2 = Rot64(s2,43);    s1 += s3;
      s3 += data[3];    s5 ^= s1;    s2 ^= s3;    s3 = Rot64(s3,31);    s2 += s4;
      s4 += data[4];    s6 ^= s2;    s3 ^= s4;    s4 = Rot64(s4,17);    s3 += s5;
      s5 += data[5];    s7 ^= s3;    s4 ^= s5;    s5 = Rot64(s5,28);    s4 += s6;
      s6 += data[6];    s8 ^= s4;    s5 ^= s6;    s6 = Rot64(s6,39);    s5 += s7;
      s7 += data[7];    s9 ^= s5;    s6 ^= s7;    s7 = Rot64(s7,57);    s6 += s8;
      s8 += data[8];    s10 ^= s6;    s7 ^= s8;    s8 = Rot64(s8,55);    s7 += s9;
      s9 += data[9];    s11 ^= s7;    s8 ^= s9;    s9 = Rot64(s9,54);    s8 += s10;
      s10 += data[10];    s0 ^= s8;    s9 ^= s10;    s10 = Rot64(s10,22);    s9 += s11;
      s11 += data[11];    s1 ^= s9;    s10 ^= s11;    s11 = Rot64(s11,46);    s10 += s0;
    }

    //
    // Mix all 12 inputs together so that h0, h1 are a hash of them all.
    //
    // For two inputs differing in just the input bits
    // Where "differ" means xor or subtraction
    // And the base value is random, or a counting value starting at that bit
    // The final result will have each bit of h0, h1 flip
    // For every input bit,
    // with probability 50 +- .3%
    // For every pair of input bits,
    // with probability 50 +- 3%
    //
    // This does not rely on the last Mix() call having already mixed some.
    // Two iterations was almost good enough for a 64-bit result, but a
    // 128-bit result is reported, so End() does three iterations.
    //
    static INLINE void EndPartial(
        uint64 &h0, uint64 &h1, uint64 &h2, uint64 &h3,
        uint64 &h4, uint64 &h5, uint64 &h6, uint64 &h7,
        uint64 &h8, uint64 &h9, uint64 &h10,uint64 &h11)
    {
        h11+= h1;    h2 ^= h11;   h1 = Rot64(h1,44);
        h0 += h2;    h3 ^= h0;    h2 = Rot64(h2,15);
        h1 += h3;    h4 ^= h1;    h3 = Rot64(h3,34);
        h2 += h4;    h5 ^= h2;    h4 = Rot64(h4,21);
        h3 += h5;    h6 ^= h3;    h5 = Rot64(h5,38);
        h4 += h6;    h7 ^= h4;    h6 = Rot64(h6,33);
        h5 += h7;    h8 ^= h5;    h7 = Rot64(h7,10);
        h6 += h8;    h9 ^= h6;    h8 = Rot64(h8,13);
        h7 += h9;    h10^= h7;    h9 = Rot64(h9,38);
        h8 += h10;   h11^= h8;    h10= Rot64(h10,53);
        h9 += h11;   h0 ^= h9;    h11= Rot64(h11,42);
        h10+= h0;    h1 ^= h10;   h0 = Rot64(h0,54);
    }

    static INLINE void End(
        const uint64 *data,
        uint64 &h0, uint64 &h1, uint64 &h2, uint64 &h3,
        uint64 &h4, uint64 &h5, uint64 &h6, uint64 &h7,
        uint64 &h8, uint64 &h9, uint64 &h10,uint64 &h11)
    {
        h0 += data[0];   h1 += data[1];   h2 += data[2];   h3 += data[3];
        h4 += data[4];   h5 += data[5];   h6 += data[6];   h7 += data[7];
        h8 += data[8];   h9 += data[9];   h10 += data[10]; h11 += data[11];
        EndPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
        EndPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
        EndPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
    }

    //
    // The goal is for each bit of the input to expand into 128 bits of
    //   apparent entropy before it is fully overwritten.
    // n trials both set and cleared at least m bits of h0 h1 h2 h3
    //   n: 2   m: 29
    //   n: 3   m: 46
    //   n: 4   m: 57
    //   n: 5   m: 107
    //   n: 6   m: 146
    //   n: 7   m: 152
    // when run forwards or backwards
    // for all 1-bit and 2-bit diffs
    // with diffs defined by either xor or subtraction
    // with a base of all zeros plus a counter, or plus another bit, or random
    //
    static INLINE void ShortMix(uint64 &h0, uint64 &h1, uint64 &h2, uint64 &h3)
    {
        h2 = Rot64(h2,50);  h2 += h3;  h0 ^= h2;
        h3 = Rot64(h3,52);  h3 += h0;  h1 ^= h3;
        h0 = Rot64(h0,30);  h0 += h1;  h2 ^= h0;
        h1 = Rot64(h1,41);  h1 += h2;  h3 ^= h1;
        h2 = Rot64(h2,54);  h2 += h3;  h0 ^= h2;
        h3 = Rot64(h3,48);  h3 += h0;  h1 ^= h3;
        h0 = Rot64(h0,38);  h0 += h1;  h2 ^= h0;
        h1 = Rot64(h1,37);  h1 += h2;  h3 ^= h1;
        h2 = Rot64(h2,62);  h2 += h3;  h0 ^= h2;
        h3 = Rot64(h3,34);  h3 += h0;  h1 ^= h3;
        h0 = Rot64(h0,5);   h0 += h1;  h2 ^= h0;
        h1 = Rot64(h1,36);  h1 += h2;  h3 ^= h1;
    }

    //
    // Mix all 4 inputs together so that h0, h1 are a hash of them all.
    //
    // For two inputs differing in just the input bits
    // Where "differ" means xor or subtraction
    // And the base value is random, or a counting value starting at that bit
    // The final result will have each bit of h0, h1 flip
    // For every input bit,
    // with probability 50 +- .3% (it is probably better than that)
    // For every pair of input bits,
    // with probability 50 +- .75% (the worst case is approximately that)
    //
    static INLINE void ShortEnd(uint64 &h0, uint64 &h1, uint64 &h2, uint64 &h3)
    {
        h3 ^= h2;  h2 = Rot64(h2,15);  h3 += h2;
        h0 ^= h3;  h3 = Rot64(h3,52);  h0 += h3;
        h1 ^= h0;  h0 = Rot64(h0,26);  h1 += h0;
        h2 ^= h1;  h1 = Rot64(h1,51);  h2 += h1;
        h3 ^= h2;  h2 = Rot64(h2,28);  h3 += h2;
        h0 ^= h3;  h3 = Rot64(h3,9);   h0 += h3;
        h1 ^= h0;  h0 = Rot64(h0,47);  h1 += h0;
        h2 ^= h1;  h1 = Rot64(h1,54);  h2 += h1;
        h3 ^= h2;  h2 = Rot64(h2,32);  h3 += h2;
        h0 ^= h3;  h3 = Rot64(h3,25);  h0 += h3;
        h1 ^= h0;  h0 = Rot64(h0,63);  h1 += h0;
    }

private:

    //
    // Short is used for messages under 192 bytes in length
    // Short has a low startup cost, the normal mode is good for long
    // keys, the cost crossover is at about 192 bytes.  The two modes were
    // held to the same quality bar.
    //
    static void Short(
        const void *message,  // message (array of bytes, not necessarily aligned)
        size_t length,        // length of message (in bytes)
        uint64 *hash1,        // in/out: in the seed, out the hash value
        uint64 *hash2);       // in/out: in the seed, out the hash value

    // number of uint64's in internal state
    static const size_t sc_numVars = 12;

    // size of the internal state
    static const size_t sc_blockSize = sc_numVars*8;

    // size of buffer of unhashed data, in bytes
    static const size_t sc_bufSize = 2*sc_blockSize;

    //
    // sc_const: a constant which:
    //  * is not zero
    //  * is odd
    //  * is a not-very-regular mix of 1's and 0's
    //  * does not need any other special mathematical properties
    //
    static const uint64 sc_const = 0xdeadbeefdeadbeefLL;

    uint64 m_data[2*sc_numVars];   // unhashed data, for partial messages
    uint64 m_state[sc_numVars];  // internal state of the hash
    size_t m_length;             // total length of the input so far
    uint8  m_remainder;          // length of unhashed data stashed in m_data
};

// =======================================
// xxHash
// https://github.com/Cyan4973/xxHash/
// =======================================

/*
   xxHash - Extremely Fast Hash algorithm
   Header File
   Copyright (C) 2012-2016, Yann Collet.
   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   You can contact the author at :
   - xxHash source repository : https://github.com/Cyan4973/xxHash
*/

/* Notice extracted from xxHash homepage :
xxHash is an extremely fast Hash algorithm, running at RAM speed limits.
It also successfully passes all tests from the SMHasher suite.
Comparison (single thread, Windows Seven 32 bits, using SMHasher on a Core 2 Duo @3GHz)
Name            Speed       Q.Score   Author
xxHash          5.4 GB/s     10
CrapWow         3.2 GB/s      2       Andrew
MumurHash 3a    2.7 GB/s     10       Austin Appleby
SpookyHash      2.0 GB/s     10       Bob Jenkins
SBox            1.4 GB/s      9       Bret Mulvey
Lookup3         1.2 GB/s      9       Bob Jenkins
SuperFastHash   1.2 GB/s      1       Paul Hsieh
CityHash64      1.05 GB/s    10       Pike & Alakuijala
FNV             0.55 GB/s     5       Fowler, Noll, Vo
CRC32           0.43 GB/s     9
MD5-32          0.33 GB/s    10       Ronald L. Rivest
SHA1-32         0.28 GB/s    10
Q.Score is a measure of quality of the hash function.
It depends on successfully passing SMHasher test set.
10 is a perfect score.
A 64-bits version, named XXH64, is available since r35.
It offers much better speed, but for 64-bits applications only.
Name     Speed on 64 bits    Speed on 32 bits
XXH64       13.8 GB/s            1.9 GB/s
XXH32        6.8 GB/s            6.0 GB/s
*/

#ifndef XXHASH_H_5627135585666179
#define XXHASH_H_5627135585666179 1

#if defined (__cplusplus)
extern "C" {
#endif


/* ****************************
*  Definitions
******************************/
#include <stddef.h>   /* size_t */
typedef enum { XXH_OK=0, XXH_ERROR } XXH_errorcode;


/* ****************************
*  API modifier
******************************/
/** XXH_PRIVATE_API
*   This is useful if you want to include xxhash functions in `static` mode
*   in order to inline them, and remove their symbol from the public list.
*   Methodology :
*     #define XXH_PRIVATE_API
*     #include "xxhash.h"
*   `xxhash.c` is automatically included, so the file is still needed,
*   but it's not useful to compile and link it anymore.
*/
#ifdef XXH_PRIVATE_API
#  ifndef XXH_STATIC_LINKING_ONLY
#    define XXH_STATIC_LINKING_ONLY
#  endif
#  if defined(__GNUC__)
#    define XXH_PUBLIC_API static __attribute__((unused))
#  elif defined (__cplusplus) || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */)
#    define XXH_PUBLIC_API static inline
#  elif defined(_MSC_VER)
#    define XXH_PUBLIC_API static __inline
#  else
#    define XXH_PUBLIC_API static   /* this version may generate warnings for unused static functions; disable the relevant warning */
#  endif
#else
#  define XXH_PUBLIC_API   /* do nothing */
#endif /* XXH_PRIVATE_API */

/*!XXH_NAMESPACE, aka Namespace Emulation :
If you want to include _and expose_ xxHash functions from within your own library,
but also want to avoid symbol collisions with another library which also includes xxHash,
you can use XXH_NAMESPACE, to automatically prefix any public symbol from xxhash library
with the value of XXH_NAMESPACE (so avoid to keep it NULL and avoid numeric values).
Note that no change is required within the calling program as long as it includes `xxhash.h` :
regular symbol name will be automatically translated by this header.
*/
#ifdef XXH_NAMESPACE
#  define XXH_CAT(A,B) A##B
#  define XXH_NAME2(A,B) XXH_CAT(A,B)
#  define XXH32 XXH_NAME2(XXH_NAMESPACE, XXH32)
#  define XXH64 XXH_NAME2(XXH_NAMESPACE, XXH64)
#  define XXH_versionNumber XXH_NAME2(XXH_NAMESPACE, XXH_versionNumber)
#  define XXH32_createState XXH_NAME2(XXH_NAMESPACE, XXH32_createState)
#  define XXH64_createState XXH_NAME2(XXH_NAMESPACE, XXH64_createState)
#  define XXH32_freeState XXH_NAME2(XXH_NAMESPACE, XXH32_freeState)
#  define XXH64_freeState XXH_NAME2(XXH_NAMESPACE, XXH64_freeState)
#  define XXH32_reset XXH_NAME2(XXH_NAMESPACE, XXH32_reset)
#  define XXH64_reset XXH_NAME2(XXH_NAMESPACE, XXH64_reset)
#  define XXH32_update XXH_NAME2(XXH_NAMESPACE, XXH32_update)
#  define XXH64_update XXH_NAME2(XXH_NAMESPACE, XXH64_update)
#  define XXH32_digest XXH_NAME2(XXH_NAMESPACE, XXH32_digest)
#  define XXH64_digest XXH_NAME2(XXH_NAMESPACE, XXH64_digest)
#  define XXH32_copyState XXH_NAME2(XXH_NAMESPACE, XXH32_copyState)
#  define XXH64_copyState XXH_NAME2(XXH_NAMESPACE, XXH64_copyState)
#endif


/* *************************************
*  Version
***************************************/
#define XXH_VERSION_MAJOR    0
#define XXH_VERSION_MINOR    6
#define XXH_VERSION_RELEASE  1
#define XXH_VERSION_NUMBER  (XXH_VERSION_MAJOR *100*100 + XXH_VERSION_MINOR *100 + XXH_VERSION_RELEASE)
XXH_PUBLIC_API unsigned XXH_versionNumber (void);


/* ****************************
*  Simple Hash Functions
******************************/
typedef unsigned int       XXH32_hash_t;
typedef unsigned long long XXH64_hash_t;

XXH_PUBLIC_API XXH32_hash_t XXH32 (const void* input, size_t length, unsigned int seed);
XXH_PUBLIC_API XXH64_hash_t XXH64 (const void* input, size_t length, unsigned long long seed);

/*!
XXH32() :
    Calculate the 32-bits hash of sequence "length" bytes stored at memory address "input".
    The memory between input & input+length must be valid (allocated and read-accessible).
    "seed" can be used to alter the result predictably.
    Speed on Core 2 Duo @ 3 GHz (single thread, SMHasher benchmark) : 5.4 GB/s
XXH64() :
    Calculate the 64-bits hash of sequence of length "len" stored at memory address "input".
    "seed" can be used to alter the result predictably.
    This function runs 2x faster on 64-bits systems, but slower on 32-bits systems (see benchmark).
*/


/* ****************************
*  Streaming Hash Functions
******************************/
typedef struct XXH32_state_s XXH32_state_t;   /* incomplete type */
typedef struct XXH64_state_s XXH64_state_t;   /* incomplete type */

/*! State allocation, compatible with dynamic libraries */

XXH_PUBLIC_API XXH32_state_t* XXH32_createState(void);
XXH_PUBLIC_API XXH_errorcode  XXH32_freeState(XXH32_state_t* statePtr);

XXH_PUBLIC_API XXH64_state_t* XXH64_createState(void);
XXH_PUBLIC_API XXH_errorcode  XXH64_freeState(XXH64_state_t* statePtr);


/* hash streaming */

XXH_PUBLIC_API XXH_errorcode XXH32_reset  (XXH32_state_t* statePtr, unsigned int seed);
XXH_PUBLIC_API XXH_errorcode XXH32_update (XXH32_state_t* statePtr, const void* input, size_t length);
XXH_PUBLIC_API XXH32_hash_t  XXH32_digest (const XXH32_state_t* statePtr);

XXH_PUBLIC_API XXH_errorcode XXH64_reset  (XXH64_state_t* statePtr, unsigned long long seed);
XXH_PUBLIC_API XXH_errorcode XXH64_update (XXH64_state_t* statePtr, const void* input, size_t length);
XXH_PUBLIC_API XXH64_hash_t  XXH64_digest (const XXH64_state_t* statePtr);

/*
These functions generate the xxHash of an input provided in multiple segments.
Note that, for small input, they are slower than single-call functions, due to state management.
For small input, prefer `XXH32()` and `XXH64()` .
XXH state must first be allocated, using XXH*_createState() .
Start a new hash by initializing state with a seed, using XXH*_reset().
Then, feed the hash state by calling XXH*_update() as many times as necessary.
Obviously, input must be allocated and read accessible.
The function returns an error code, with 0 meaning OK, and any other value meaning there is an error.
Finally, a hash value can be produced anytime, by using XXH*_digest().
This function returns the nn-bits hash as an int or long long.
It's still possible to continue inserting input into the hash state after a digest,
and generate some new hashes later on, by calling again XXH*_digest().
When done, free XXH state space if it was allocated dynamically.
*/


/* **************************
*  Utils
****************************/
#if !(defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))   /* ! C99 */
#  define restrict   /* disable restrict */
#endif

XXH_PUBLIC_API void XXH32_copyState(XXH32_state_t* restrict dst_state, const XXH32_state_t* restrict src_state);
XXH_PUBLIC_API void XXH64_copyState(XXH64_state_t* restrict dst_state, const XXH64_state_t* restrict src_state);


/* **************************
*  Canonical representation
****************************/
typedef struct { unsigned char digest[4]; } XXH32_canonical_t;
typedef struct { unsigned char digest[8]; } XXH64_canonical_t;

XXH_PUBLIC_API void XXH32_canonicalFromHash(XXH32_canonical_t* dst, XXH32_hash_t hash);
XXH_PUBLIC_API void XXH64_canonicalFromHash(XXH64_canonical_t* dst, XXH64_hash_t hash);

XXH_PUBLIC_API XXH32_hash_t XXH32_hashFromCanonical(const XXH32_canonical_t* src);
XXH_PUBLIC_API XXH64_hash_t XXH64_hashFromCanonical(const XXH64_canonical_t* src);

/* Default result type for XXH functions are primitive unsigned 32 and 64 bits.
*  The canonical representation uses human-readable write convention, aka big-endian (large digits first).
*  These functions allow transformation of hash result into and from its canonical format.
*  This way, hash values can be written into a file / memory, and remain comparable on different systems and programs.
*/


#ifdef XXH_STATIC_LINKING_ONLY

/* ================================================================================================
   This section contains definitions which are not guaranteed to remain stable.
   They could change in a future version, becoming incompatible with a different version of the library.
   They shall only be used with static linking.
=================================================================================================== */

/* These definitions allow allocating XXH state statically (on stack) */

   struct XXH32_state_s {
       unsigned long long total_len;
       unsigned seed;
       unsigned v1;
       unsigned v2;
       unsigned v3;
       unsigned v4;
       unsigned mem32[4];   /* buffer defined as U32 for alignment */
       unsigned memsize;
   };   /* typedef'd to XXH32_state_t */

   struct XXH64_state_s {
       unsigned long long total_len;
       unsigned long long seed;
       unsigned long long v1;
       unsigned long long v2;
       unsigned long long v3;
       unsigned long long v4;
       unsigned long long mem64[4];   /* buffer defined as U64 for alignment */
       unsigned memsize;
   };   /* typedef'd to XXH64_state_t */


#  ifdef XXH_PRIVATE_API
#    include "xxhash.c"   /* include xxhash functions as `static`, for inlining */
#  endif

#endif /* XXH_STATIC_LINKING_ONLY */


#if defined (__cplusplus)
}
#endif

#endif /* XXHASH_H_5627135585666179 */



// Hash Functions all the way down
// http://aras-p.info/blog/2016/08/02/Hash-Functions-all-the-way-down/
// * xxHash64 is really good, especially if you’re on an 64 bit Intel CPU.
// * If you need 128 bit keys, use SpookyHash. It’s also really good if you’re
// on a non-Intel 64 bit CPU (as shown by XboxOne - AMD and iPhone6 - ARM
// throughput tests).
// * If you need a 32 bit hash, and are on a 32 bit CPU/system, do not use
// xxHash64 or SpookyHash! Their 64 bit math is costly when on 32 bit; use
// xxHash32 or Murmur.
// * For short data/strings, simplicity of FNV or djb2 are hard to beat, they
// are very performant on short data as well.

// Hash functions: An empirical comparison
// https://www.strchr.com/hash_functions





#endif  // MING_NONCOPYABLE_H_
