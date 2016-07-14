#include "encoding.h"
#include <iconv.h>
#include <errno.h>
#include <iomanip>

#if _MSC_VER < 1600
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#else
#include <stdint.h>
#endif

#include <stdio.h>

namespace ming {
namespace encoding {

int charset_gsm_to_utf8(unsigned char *dest, int *dest_len,
                        const unsigned char *src, int *src_len);
int charset_utf8_to_gsm(unsigned char *dest, int *dest_len,
                        const unsigned char *src, int *src_len);
int isolat1ToUTF8(unsigned char *out, int *outlen, const unsigned char *in,
                  int *inlen);
int UTF8Toisolat1(unsigned char *out, int *outlen, const unsigned char *in,
                  int *inlen);
int UTF16LEToUTF8(unsigned char *out, int *outlen, const unsigned char *inb,
                  int *inlenb);
int UTF8ToUTF16LE(unsigned char *outb, int *outlen, const unsigned char *in,
                  int *inlen);
int UTF16BEToUTF8(unsigned char *out, int *outlen, const unsigned char *inb,
                  int *inlenb);
int UTF8ToUTF16BE(unsigned char *outb, int *outlen, const unsigned char *in,
                  int *inlen);

// for raw bytes doesn't end with '\0'
int latin1_to_utf8(char *dest, int *dest_len, const char *src, int *src_len) {
  return isolat1ToUTF8((unsigned char *)dest, dest_len,
                       (const unsigned char *)src, src_len);
}

int utf8_to_latin1(char *dest, int *dest_len, const char *src, int *src_len) {
  return UTF8Toisolat1((unsigned char *)dest, dest_len,
                       (const unsigned char *)src, src_len);
}

int utf8_to_ucs2(char *outb, int *outlen, const char *in, int *inlen) {
  return UTF8ToUTF16BE((unsigned char *)outb, outlen, (const unsigned char *)in,
                       inlen);
}

int ucs2_to_utf8(char *outb, int *outlen, const char *in, int *inlen) {
  return UTF16BEToUTF8((unsigned char *)outb, outlen, (const unsigned char *)in,
                       inlen);
}

int utf8_to_ucs2le(char *outb, int *outlen, const char *in, int *inlen) {
  return UTF8ToUTF16LE((unsigned char *)outb, outlen, (const unsigned char *)in,
                       inlen);
}

int ucs2le_to_utf8(char *outb, int *outlen, const char *in, int *inlen) {
  return UTF16LEToUTF8((unsigned char *)outb, outlen, (const unsigned char *)in,
                       inlen);
}

int gsm_to_utf8(char *dest, int *dest_len, const char *src, int *src_len) {
  return charset_gsm_to_utf8((unsigned char *)dest, dest_len,
                             (const unsigned char *)src, src_len);
}
int utf8_to_gsm(char *dest, int *dest_len, const char *src, int *src_len) {
  return charset_utf8_to_gsm((unsigned char *)dest, dest_len,
                             (const unsigned char *)src, src_len);
}

//===========================================================
// hex encoding
//===========================================================
/**
 * \file
 * <PRE>
 * MODP_B16 - High performance base16 encoder/decoder
 * http://code.google.com/p/stringencoders/
 *
 * Copyright &copy; 2005, 2006, 2007  Nick Galbreath -- nickg [at] modp [dot]
 * com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the modp.com nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This is the standard "new" BSD license:
 * http://www.opensource.org/licenses/bsd-license.php
 * </PRE>
 */

static const uint32_t hexDecodeMap1[256] = {
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    0,    16,   32,   48,   64,   80,   96,   112,  128,  144,  4096, 4096,
    4096, 4096, 4096, 4096, 4096, 160,  176,  192,  208,  224,  240,  4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 160,  176,  192,  208,  224,  240,  4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096,
    4096, 4096, 4096, 4096};

static const uint32_t hexDecodeMap2[256] = {
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   256, 256,
    256, 256, 256, 256, 256, 10,  11,  12,  13,  14,  15,  256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 10,  11,  12,  13,  14,  15,  256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    256};

static const unsigned char hexEncodeMap1[256] = {
    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
    '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
    '1', '1', '2', '2', '2', '2', '2', '2', '2', '2', '2', '2', '2', '2', '2',
    '2', '2', '2', '3', '3', '3', '3', '3', '3', '3', '3', '3', '3', '3', '3',
    '3', '3', '3', '3', '4', '4', '4', '4', '4', '4', '4', '4', '4', '4', '4',
    '4', '4', '4', '4', '4', '5', '5', '5', '5', '5', '5', '5', '5', '5', '5',
    '5', '5', '5', '5', '5', '5', '6', '6', '6', '6', '6', '6', '6', '6', '6',
    '6', '6', '6', '6', '6', '6', '6', '7', '7', '7', '7', '7', '7', '7', '7',
    '7', '7', '7', '7', '7', '7', '7', '7', '8', '8', '8', '8', '8', '8', '8',
    '8', '8', '8', '8', '8', '8', '8', '8', '8', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', 'A', 'A', 'A', 'A', 'A',
    'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'B', 'B', 'B', 'B',
    'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'C', 'C', 'C',
    'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'D', 'D',
    'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'E',
    'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E',
    'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F',
    'F'};

static const unsigned char hexEncodeMap2[256] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    'F', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D',
    'E', 'F', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C',
    'D', 'E', 'F', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
    'B', 'C', 'D', 'E', 'F', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', '0', '1', '2', '3', '4', '5', '6', '7', '8',
    '9', 'A', 'B', 'C', 'D', 'E', 'F', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '0', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '0', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '0', '1', '2',
    '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '0', '1',
    '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '0',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    'F'};

int hex_encode(char *dest, const char *str, int len) {
#ifdef WORDS_BIGENDIAN
  unsigned char *src_ = (unsigned char *)src;
  for (int i = 0; i < len; i++) {
    unsigned char v = src_[i];
    dst[i * 2] = hextable[v >> 4];
    dst[i * 2 + 1] = hextable[v & 0x0f];
  }
  dst[len * 2] = '\0';
  return len * 2;
#else

  const int buckets = len >> 2;     // i.e. i / 4
  const int leftover = len & 0x03;  // i.e. i % 4

  uint8_t *p = (uint8_t *)dest;
  uint32_t *srcInt = (uint32_t *)str;

  for (int i = 0; i < buckets; ++i) {
    uint32_t x = *srcInt++;
    *p++ = hexEncodeMap1[x & 0xff];
    *p++ = hexEncodeMap2[x & 0xff];
    x >>= 8;
    *p++ = hexEncodeMap1[x & 0xff];
    *p++ = hexEncodeMap2[x & 0xff];
    x >>= 8;
    *p++ = hexEncodeMap1[x & 0xff];
    *p++ = hexEncodeMap2[x & 0xff];
    x >>= 8;
    *p++ = hexEncodeMap1[x];
    *p++ = hexEncodeMap2[x];
  }

  uint8_t *s = (uint8_t *)srcInt;
  uint8_t v;
  switch (leftover) {
    case 3:
      v = *s++;
      *p++ = hexEncodeMap1[v];
      *p++ = hexEncodeMap2[v];
    case 2:
      v = *s++;
      *p++ = hexEncodeMap1[v];
      *p++ = hexEncodeMap2[v];
    case 1:
      v = *s++;
      *p++ = hexEncodeMap1[v];
      *p++ = hexEncodeMap2[v];
    case 0:
    default:;
  }

  *p = '\0';
  return len * 2;
#endif
}

int hex_decode(char *dest, const char *str, int len) {
  int i;

  uint32_t val1, val2;
  uint8_t *p = (uint8_t *)dest;
  uint8_t *s = (uint8_t *)str;

  const int buckets = len >> 2;     // i.e. len / 4
  const int leftover = len & 0x03;  // i.e. len % 4
  if (leftover & 0x01) {            // i.e if leftover is odd,
    // leftover==1 || leftover == 3
    return -1;
  }

  // read 4 bytes, output 2.
  // Note on PPC G4, GCC 4.0, it's quite a bit faster to
  // NOT use t0,t1,t2,t3, and just put the *s++ in the gsHexDecodeMap
  // lookup
  uint8_t t0, t1, t2, t3;
  for (i = 0; i < buckets; ++i) {
    t0 = *s++;
    t1 = *s++;
    t2 = *s++;
    t3 = *s++;
    val1 = hexDecodeMap1[t0] | hexDecodeMap2[t1];
    val2 = hexDecodeMap1[t2] | hexDecodeMap2[t3];
    if (val1 > 0xff || val2 > 0xff) return -1;
    *p++ = (uint8_t)val1;
    *p++ = (uint8_t)val2;
  }

  if (leftover == 2) {
    val1 = hexDecodeMap1[s[0]] | hexDecodeMap2[s[1]];
    if (val1 > 0xff) return -1;
    *p++ = (uint8_t)val1;
  }

  return (int)(p - (uint8_t *)dest);
}

// for c string end with '\0'
int latin1_to_utf8(char *dest, char const *src) {
  if (src == NULL || dest == NULL) return -1;

  if (src[0] == 0) {
    *dest = 0;
    return 0;
  }
  char *out_start = dest;
  while (*src) {
    uint8_t ch = (uint8_t)*src++;
    if (ch <= 0x7F) {  // ch is an ASCII
      *dest++ = (char)ch;
    } else {  // ch is an Latin1 extension
      uint8_t c0, c1;
      c1 = ((ch & 0x3F) | 0x80);
      ch >>= 6;
      ch |= 0xC0;
      c0 = ch;

      *dest++ = (char)c0;
      *dest++ = (char)c1;
    }
  }
  // add the ending NULL
  *dest = '\0';

  return dest - out_start;
}

//=====================================================================
// GSM to utf-8 encoding function from Kannel
//=====================================================================
/* ====================================================================
 * The Kannel Software License, Version 1.0
 *
 * Copyright (c) 2001-2010 Kannel Group
 * Copyright (c) 1998-2001 WapIT Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Kannel Group (http://www.kannel.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Kannel" and "Kannel Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please
 *    contact org@kannel.org.
 *
 * 5. Products derived from this software may not be called "Kannel",
 *    nor may "Kannel" appear in their name, without prior written
 *    permission of the Kannel Group.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE KANNEL GROUP OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Kannel Group.  For more information on
 * the Kannel Group, please see <http://www.kannel.org/>.
 *
 * Portions of this software are based upon software originally written at
 * WapIT Ltd., Helsinki, Finland for the Kannel project.
 */

/*
 * gwlib/charset.c - character set conversions
 *
 * This file implements the character set conversions declared in charset.h.
 *
 * Richard Braakman
 */

/* Code used for non-representable characters */
#define NRP '?'

static const int latin1_to_gsm[256] = {
    /* 0x00 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x01 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x02 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x03 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x04 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x05 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x06 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x07 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x08 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x09 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x0a */ 0x0a,  /* pc: NON PRINTABLE */
    /* 0x0b */ NRP,   /* pc: NON PRINTABLE */
    /* 0x0c */ -0x0a, /* pc: NON PRINTABLE */
    /* 0x0d */ 0x0d,  /* pc: NON PRINTABLE */
    /* 0x0e */ NRP,   /* pc: NON PRINTABLE */
    /* 0x0f */ NRP,   /* pc: NON PRINTABLE */
    /* 0x10 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x11 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x12 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x13 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x14 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x15 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x16 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x17 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x18 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x19 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x1a */ NRP,   /* pc: NON PRINTABLE */
    /* 0x1b */ NRP,   /* pc: NON PRINTABLE */
    /* 0x1c */ NRP,   /* pc: NON PRINTABLE */
    /* 0x1d */ NRP,   /* pc: NON PRINTABLE */
    /* 0x1e */ NRP,   /* pc: NON PRINTABLE */
    /* 0x1f */ NRP,   /* pc: NON PRINTABLE */
    /* 0x20 */ 0x20,  /* pc:   */
    /* 0x21 */ 0x21,  /* pc: ! */
    /* 0x22 */ 0x22,  /* pc: " */
    /* 0x23 */ 0x23,  /* pc: # */
    /* 0x24 */ 0x02,  /* pc: $ */
    /* 0x25 */ 0x25,  /* pc: % */
    /* 0x26 */ 0x26,  /* pc: & */
    /* 0x27 */ 0x27,  /* pc: ' */
    /* 0x28 */ 0x28,  /* pc: ( */
    /* 0x29 */ 0x29,  /* pc: ) */
    /* 0x2a */ 0x2a,  /* pc: * */
    /* 0x2b */ 0x2b,  /* pc: + */
    /* 0x2c */ 0x2c,  /* pc: , */
    /* 0x2d */ 0x2d,  /* pc: - */
    /* 0x2e */ 0x2e,  /* pc: . */
    /* 0x2f */ 0x2f,  /* pc: / */
    /* 0x30 */ 0x30,  /* pc: 0 */
    /* 0x31 */ 0x31,  /* pc: 1 */
    /* 0x32 */ 0x32,  /* pc: 2 */
    /* 0x33 */ 0x33,  /* pc: 3 */
    /* 0x34 */ 0x34,  /* pc: 4 */
    /* 0x35 */ 0x35,  /* pc: 5 */
    /* 0x36 */ 0x36,  /* pc: 6 */
    /* 0x37 */ 0x37,  /* pc: 7 */
    /* 0x38 */ 0x38,  /* pc: 8 */
    /* 0x39 */ 0x39,  /* pc: 9 */
    /* 0x3a */ 0x3a,  /* pc: : */
    /* 0x3b */ 0x3b,  /* pc: ; */
    /* 0x3c */ 0x3c,  /* pc: < */
    /* 0x3d */ 0x3d,  /* pc: = */
    /* 0x3e */ 0x3e,  /* pc: > */
    /* 0x3f */ 0x3f,  /* pc: ? */
    /* 0x40 */ 0x00,  /* pc: @ */
    /* 0x41 */ 0x41,  /* pc: A */
    /* 0x42 */ 0x42,  /* pc: B */
    /* 0x43 */ 0x43,  /* pc: C */
    /* 0x44 */ 0x44,  /* pc: D */
    /* 0x45 */ 0x45,  /* pc: E */
    /* 0x46 */ 0x46,  /* pc: F */
    /* 0x47 */ 0x47,  /* pc: G */
    /* 0x48 */ 0x48,  /* pc: H */
    /* 0x49 */ 0x49,  /* pc: I */
    /* 0x4a */ 0x4a,  /* pc: J */
    /* 0x4b */ 0x4b,  /* pc: K */
    /* 0x4c */ 0x4c,  /* pc: L */
    /* 0x4d */ 0x4d,  /* pc: M */
    /* 0x4e */ 0x4e,  /* pc: N */
    /* 0x4f */ 0x4f,  /* pc: O */
    /* 0x50 */ 0x50,  /* pc: P */
    /* 0x51 */ 0x51,  /* pc: Q */
    /* 0x52 */ 0x52,  /* pc: R */
    /* 0x53 */ 0x53,  /* pc: S */
    /* 0x54 */ 0x54,  /* pc: T */
    /* 0x55 */ 0x55,  /* pc: U */
    /* 0x56 */ 0x56,  /* pc: V */
    /* 0x57 */ 0x57,  /* pc: W */
    /* 0x58 */ 0x58,  /* pc: X */
    /* 0x59 */ 0x59,  /* pc: Y */
    /* 0x5a */ 0x5a,  /* pc: Z */
    /* 0x5b */ -0x3c, /* pc: [ */
    /* 0x5c */ -0x2f, /* pc: \ */
    /* 0x5d */ -0x3e, /* pc: ] */
    /* 0x5e */ -0x14, /* pc: ^ */
    /* 0x5f */ 0x11,  /* pc: _ */
    /* 0x60 */ NRP,   /* pc: ` */
    /* 0x61 */ 0x61,  /* pc: a */
    /* 0x62 */ 0x62,  /* pc: b */
    /* 0x63 */ 0x63,  /* pc: c */
    /* 0x64 */ 0x64,  /* pc: d */
    /* 0x65 */ 0x65,  /* pc: e */
    /* 0x66 */ 0x66,  /* pc: f */
    /* 0x67 */ 0x67,  /* pc: g */
    /* 0x68 */ 0x68,  /* pc: h */
    /* 0x69 */ 0x69,  /* pc: i */
    /* 0x6a */ 0x6a,  /* pc: j */
    /* 0x6b */ 0x6b,  /* pc: k */
    /* 0x6c */ 0x6c,  /* pc: l */
    /* 0x6d */ 0x6d,  /* pc: m */
    /* 0x6e */ 0x6e,  /* pc: n */
    /* 0x6f */ 0x6f,  /* pc: o */
    /* 0x70 */ 0x70,  /* pc: p */
    /* 0x71 */ 0x71,  /* pc: q */
    /* 0x72 */ 0x72,  /* pc: r */
    /* 0x73 */ 0x73,  /* pc: s */
    /* 0x74 */ 0x74,  /* pc: t */
    /* 0x75 */ 0x75,  /* pc: u */
    /* 0x76 */ 0x76,  /* pc: v */
    /* 0x77 */ 0x77,  /* pc: w */
    /* 0x78 */ 0x78,  /* pc: x */
    /* 0x79 */ 0x79,  /* pc: y */
    /* 0x7a */ 0x7a,  /* pc: z */
    /* 0x7b */ -0x28, /* pc: { */
    /* 0x7c */ -0x40, /* pc: | */
    /* 0x7d */ -0x29, /* pc: } */
    /* 0x7e */ -0x3d, /* pc: ~ */
    /* 0x7f */ NRP,   /* pc: NON PRINTABLE */
    /* 0x80 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x81 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x82 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x83 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x84 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x85 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x86 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x87 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x88 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x89 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x8a */ NRP,   /* pc: NON PRINTABLE */
    /* 0x8b */ NRP,   /* pc: NON PRINTABLE */
    /* 0x8c */ NRP,   /* pc: NON PRINTABLE */
    /* 0x8d */ NRP,   /* pc: NON PRINTABLE */
    /* 0x8e */ NRP,   /* pc: NON PRINTABLE */
    /* 0x8f */ NRP,   /* pc: NON PRINTABLE */
    /* 0x90 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x91 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x92 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x93 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x94 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x95 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x96 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x97 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x98 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x99 */ NRP,   /* pc: NON PRINTABLE */
    /* 0x9a */ NRP,   /* pc: NON PRINTABLE */
    /* 0x9b */ NRP,   /* pc: NON PRINTABLE */
    /* 0x9c */ NRP,   /* pc: NON PRINTABLE */
    /* 0x9d */ NRP,   /* pc: NON PRINTABLE */
    /* 0x9e */ NRP,   /* pc: NON PRINTABLE */
    /* 0x9f */ NRP,   /* pc: NON PRINTABLE */
    /* 0xa0 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xa1 */ 0x40,  /* pc: INVERTED EXCLAMATION MARK */
    /* 0xa2 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xa3 */ 0x01,  /* pc: POUND SIGN */
    /* 0xa4 */ 0x24,  /* pc: CURRENCY SIGN */
    /* 0xa5 */ 0x03,  /* pc: YEN SIGN*/
    /* 0xa6 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xa7 */ 0x5f,  /* pc: SECTION SIGN */
    /* 0xa8 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xa9 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xaa */ NRP,   /* pc: NON PRINTABLE */
    /* 0xab */ NRP,   /* pc: NON PRINTABLE */
    /* 0xac */ NRP,   /* pc: NON PRINTABLE */
    /* 0xad */ NRP,   /* pc: NON PRINTABLE */
    /* 0xae */ NRP,   /* pc: NON PRINTABLE */
    /* 0xaf */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb0 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb1 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb2 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb3 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb4 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb5 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb6 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb7 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb8 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xb9 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xba */ NRP,   /* pc: NON PRINTABLE */
    /* 0xbb */ NRP,   /* pc: NON PRINTABLE */
    /* 0xbc */ NRP,   /* pc: NON PRINTABLE */
    /* 0xbd */ NRP,   /* pc: NON PRINTABLE */
    /* 0xbe */ NRP,   /* pc: NON PRINTABLE */
    /* 0xbf */ 0x60,  /* pc: INVERTED QUESTION MARK */
    /* 0xc0 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xc1 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xc2 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xc3 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xc4 */ 0x5b,  /* pc: LATIN CAPITAL LETTER A WITH DIAERESIS */
    /* 0xc5 */ 0x0e,  /* pc: LATIN CAPITAL LETTER A WITH RING ABOVE */
    /* 0xc6 */ 0x1c,  /* pc: LATIN CAPITAL LETTER AE */
    /* 0xc7 */ 0x09,  /* pc: LATIN CAPITAL LETTER C WITH CEDILLA (mapped to
                         small) */
    /* 0xc8 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xc9 */ 0x1f,  /* pc: LATIN CAPITAL LETTER E WITH ACUTE  */
    /* 0xca */ NRP,   /* pc: NON PRINTABLE */
    /* 0xcb */ NRP,   /* pc: NON PRINTABLE */
    /* 0xcc */ NRP,   /* pc: NON PRINTABLE */
    /* 0xcd */ NRP,   /* pc: NON PRINTABLE */
    /* 0xce */ NRP,   /* pc: NON PRINTABLE */
    /* 0xcf */ NRP,   /* pc: NON PRINTABLE */
    /* 0xd0 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xd1 */ 0x5d,  /* pc: LATIN CAPITAL LETTER N WITH TILDE */
    /* 0xd2 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xd3 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xd4 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xd5 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xd6 */ 0x5c,  /* pc: LATIN CAPITAL LETTER O WITH DIAEREIS */
    /* 0xd7 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xd8 */ 0x0b,  /* pc: LATIN CAPITAL LETTER O WITH STROKE */
    /* 0xd9 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xda */ NRP,   /* pc: NON PRINTABLE */
    /* 0xdb */ NRP,   /* pc: NON PRINTABLE */
    /* 0xdc */ 0x5e,  /* pc: LATIN CAPITAL LETTER U WITH DIAERESIS */
    /* 0xdd */ NRP,   /* pc: NON PRINTABLE */
    /* 0xde */ NRP,   /* pc: NON PRINTABLE */
    /* 0xdf */ 0x1e,  /* pc: LATIN SMALL LETTER SHARP S */
    /* 0xe0 */ 0x7f,  /* pc: LATIN SMALL LETTER A WITH GRAVE */
    /* 0xe1 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xe2 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xe3 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xe4 */ 0x7b,  /* pc: LATIN SMALL LETTER A WITH DIAERESIS */
    /* 0xe5 */ 0x0f,  /* pc: LATIN SMALL LETTER A WITH RING ABOVE */
    /* 0xe6 */ 0x1d,  /* pc: LATIN SMALL LETTER AE */
    /* 0xe7 */ 0x09,  /* pc: LATIN SMALL LETTER C WITH CEDILLA */
    /* 0xe8 */ 0x04,  /* pc: NON PRINTABLE */
    /* 0xe9 */ 0x05,  /* pc: NON PRINTABLE */
    /* 0xea */ NRP,   /* pc: NON PRINTABLE */
    /* 0xeb */ NRP,   /* pc: NON PRINTABLE */
    /* 0xec */ 0x07,  /* pc: NON PRINTABLE */
    /* 0xed */ NRP,   /* pc: NON PRINTABLE */
    /* 0xee */ NRP,   /* pc: NON PRINTABLE */
    /* 0xef */ NRP,   /* pc: NON PRINTABLE */
    /* 0xf0 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xf1 */ 0x7d,  /* pc: NON PRINTABLE */
    /* 0xf2 */ 0x08,  /* pc: NON PRINTABLE */
    /* 0xf3 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xf4 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xf5 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xf6 */ 0x7c,  /* pc: NON PRINTABLE */
    /* 0xf7 */ NRP,   /* pc: NON PRINTABLE */
    /* 0xf8 */ 0x0c,  /* pc: NON PRINTABLE */
    /* 0xf9 */ 0x06,  /* pc: NON PRINTABLE */
    /* 0xfa */ NRP,   /* pc: NON PRINTABLE */
    /* 0xfb */ NRP,   /* pc: NON PRINTABLE */
    /* 0xfc */ 0x7e,  /* pc: NON PRINTABLE */
    /* 0xfd */ NRP,   /* pc: NON PRINTABLE */
    /* 0xfe */ NRP,   /* pc: NON PRINTABLE */
    /* 0xff */ NRP,   /* pc: NON PRINTABLE */
};

/**
 * Struct maps escaped GSM chars to unicode codeposition.
 */
static const struct {
  int gsmesc;
  int unichar;
} gsm_esctouni[] = {
    {10, 12}, /* ASCII page break */
    {20, '^'}, {40, '{'}, {41, '}'}, {47, '\\'},    {60, '['},
    {61, '~'}, {62, ']'}, {64, '|'}, {'e', 0x20AC}, /* euro symbol */
    {-1, -1}};

/**
 * Map GSM default alphabet characters to unicode codeposition.
 * The escape character, at position 27, is mapped to a NRP,
 * though normally the function that indexes into this table will
 * treat it specially.
 */
static const int gsm_to_unicode[128] = {
    '@',   0xA3,  '$',   0xA5,  0xE8,
    0xE9,  0xF9,  0xEC, /* 0 - 7 */
    0xF2,  0xC7,  10,    0xd8,  0xF8,
    13,    0xC5,  0xE5, /* 8 - 15 */
    0x394, '_',   0x3A6, 0x393, 0x39B,
    0x3A9, 0x3A0, 0x3A8, /* 16 - 23 */
    0x3A3, 0x398, 0x39E, NRP,   0xC6,
    0xE6,  0xDF,  0xC9, /* 24 - 31 */
    ' ',   '!',   '"',   '#',   0xA4,
    '%',   '&',   '\'', /* 32 - 39 */
    '(',   ')',   '*',   '+',   ',',
    '-',   '.',   '/', /* 40 - 47 */
    '0',   '1',   '2',   '3',   '4',
    '5',   '6',   '7', /* 48 - 55 */
    '8',   '9',   ':',   ';',   '<',
    '=',   '>',   '?', /* 56 - 63 */
    0xA1,  'A',   'B',   'C',   'D',
    'E',   'F',   'G', /* 64 - 71 */
    'H',   'I',   'J',   'K',   'L',
    'M',   'N',   'O', /* 73 - 79 */
    'P',   'Q',   'R',   'S',   'T',
    'U',   'V',   'W', /* 80 - 87 */
    'X',   'Y',   'Z',   0xC4,  0xD6,
    0xD1,  0xDC,  0xA7, /* 88 - 95 */
    0xBF,  'a',   'b',   'c',   'd',
    'e',   'f',   'g', /* 96 - 103 */
    'h',   'i',   'j',   'k',   'l',
    'm',   'n',   'o', /* 104 - 111 */
    'p',   'q',   'r',   's',   't',
    'u',   'v',   'w', /* 112 - 119 */
    'x',   'y',   'z',   0xE4,  0xF6,
    0xF1,  0xFC,  0xE0 /* 120 - 127 */
};

/**
 * Convert octet string in GSM format to UTF-8.
 * Every GSM character can be represented with unicode, hence nothing will
 * be lost. Escaped charaters will be translated into appropriate UTF-8
 * character.
 */
int charset_gsm_to_utf8(unsigned char *dest, int *dest_len,
                        const unsigned char *src, int *src_len) {
  long pos, len;
  unsigned char *out;
  unsigned char *out_end;

  if (src == NULL || dest == NULL || dest_len == NULL || src_len == NULL)
    return -1;

  out = dest;
  out_end = dest + *dest_len;
  len = *src_len;

  for (pos = 0; pos < len; pos++) {
    int c, i;

    c = src[pos];
    if (c > 127) {
      // warning(0, "Could not convert GSM (0x%02x) to Unicode.", c);
      continue;
    }

    if (c == 27 && pos + 1 < len) {
      c = src[++pos];
      for (i = 0; gsm_esctouni[i].gsmesc >= 0; i++) {
        if (gsm_esctouni[i].gsmesc == c) break;
      }
      if (gsm_esctouni[i].gsmesc == c) {
        /* found a value for escaped char */
        c = gsm_esctouni[i].unichar;
      } else {
        /* nothing found, look esc in our table */
        c = gsm_to_unicode[27];
        pos--;
      }
    } else if (c < 128) {
      c = gsm_to_unicode[c];
    }

    /* unicode to utf-8 */
    if (c < 128) {
      /* 0-127 are ASCII chars that need no conversion */
      if (dest >= out_end) {
        break;
      }
      *dest++ = c;
    } else {
      /* test if it can be converterd into a two byte char */
      if (c < 0x0800) {
        if (dest + 1 >= out_end) {
          break;
        }
        *dest++ = (unsigned char)((c >> 6) | 0xC0) & 0xFF; /* add 110xxxxx */
        *dest++ = (unsigned char)(c & 0x3F) | 0x80;        /* add 10xxxxxx */
      } else {
        if (dest + 2 >= out_end) {
          break;
        }
        /* else we encode with 3 bytes. This only happens in case of euro symbol
         */
        *dest++ = (unsigned char)((c >> 12) | 0xE0) & 0xFF; /* add 1110xxxx */
        *dest++ =
            (unsigned char)(((c >> 6) & 0x3F) | 0x80) & 0xFF; /* add 10xxxxxx */
        *dest++ = (unsigned char)((c & 0x3F) | 0x80) & 0xFF;  /* add 10xxxxxx */
      }
      /* There are no 4 bytes encoded characters in GSM charset */
    }
  }

  *dest_len = dest - out;
  *src_len = pos;

  return *dest_len;
}

/**
 * Convert octet string in UTF-8 format to GSM 03.38.
 * Because not all UTF-8 charater can be converted to GSM 03.38 non
 * convertable character replaces with NRP character (see define above).
 * Special characters will be formed into escape sequences.
 * Incomplete UTF-8 characters at the end of the string will be skipped.
 */
int charset_utf8_to_gsm(unsigned char *dest, int *dest_len,
                        const unsigned char *src, int *src_len) {
  long pos, len;
  int val1, val2;
  unsigned char *out;
  unsigned char *out_end;

  if (src == NULL || dest == NULL || dest_len == NULL || src_len == NULL)
    return -1;

  out = dest;
  out_end = dest + *dest_len;
  len = *src_len;

  for (pos = 0; pos < len; pos++) {
    val1 = src[pos];

    /* check range */
    if (val1 < 0 || val1 > 255) {
      // warning(0, "Char (0x%02x) in UTF-8 string not in the range (0, 255).
      // Skipped.", val1);
      continue;
    }

    /* Convert UTF-8 to unicode code */

    /* test if two byte utf8 char */
    if ((val1 & 0xE0) == 0xC0) {
      /* test if incomplete utf char */
      if (pos + 1 < len) {
        val2 = src[++pos];
        val1 = (((val1 & ~0xC0) << 6) | (val2 & 0x3F));
      } else {
        /* incomplete, ignore it */
        // warning(0, "Incomplete UTF-8 char discovered, skipped. 1");
        pos += 1;
        continue;
      }
    } else if ((val1 & 0xF0) == 0xE0) { /* test for three byte utf8 char */
      if (pos + 2 < len) {
        val2 = src[++pos];
        val1 = (((val1 & ~0xE0) << 6) | (val2 & 0x3F));
        val2 = src[++pos];
        val1 = (val1 << 6) | (val2 & 0x3F);
      } else {
        /* incomplete, ignore it */
        // warning(0, "Incomplete UTF-8 char discovered, skipped. 2");
        pos += 2;
        continue;
      }
    }

    /* test Latin code page 1 char */
    if (val1 <= 255) {
      val1 = latin1_to_gsm[val1];
      /* needs to be escaped ? */
      if (val1 < 0) {
        if (dest >= out_end) {
          break;
        }
        *dest++ = (unsigned char)27;
        val1 *= -1;
      }
    } else {
      /* Its not a Latin1 char, test for allowed GSM chars */
      switch (val1) {
        case 0x394:
          val1 = 0x10; /* GREEK CAPITAL LETTER DELTA */
          break;
        case 0x3A6:
          val1 = 0x12; /* GREEK CAPITAL LETTER PHI */
          break;
        case 0x393:
          val1 = 0x13; /* GREEK CAPITAL LETTER GAMMA */
          break;
        case 0x39B:
          val1 = 0x14; /* GREEK CAPITAL LETTER LAMBDA */
          break;
        case 0x3A9:
          val1 = 0x15; /* GREEK CAPITAL LETTER OMEGA */
          break;
        case 0x3A0:
          val1 = 0x16; /* GREEK CAPITAL LETTER PI */
          break;
        case 0x3A8:
          val1 = 0x17; /* GREEK CAPITAL LETTER PSI */
          break;
        case 0x3A3:
          val1 = 0x18; /* GREEK CAPITAL LETTER SIGMA */
          break;
        case 0x398:
          val1 = 0x19; /* GREEK CAPITAL LETTER THETA */
          break;
        case 0x39E:
          val1 = 0x1A; /* GREEK CAPITAL LETTER XI */
          break;
        case 0x20AC:
          val1 = 'e'; /* EURO SIGN */
          if (dest >= out_end) {
            break;
          }
          *dest++ = (unsigned char)27;
          break;
        default:
          val1 = NRP; /* character cannot be represented in GSM 03.38 */
      }
    }

    if (dest >= out_end) {
      break;
    }
    *dest++ = (unsigned char)val1;
  }

  *dest_len = dest - out;
  *src_len = pos;

  return *dest_len;
}

//=======================================================================
//    UCS2 encoding function from libxml2 , MIT license
//=======================================================================

/*
 * encoding.c : implements the encoding conversion functions needed for XML
 *
 * Related specs:
 * rfc2044        (UTF-8 and UTF-16) F. Yergeau Alis Technologies
 * rfc2781        UTF-16, an encoding of ISO 10646, P. Hoffman, F. Yergeau
 * [ISO-10646]    UTF-8 and UTF-16 in Annexes
 * [ISO-8859-1]   ISO Latin-1 characters codes.
 * [UNICODE]      The Unicode Consortium, "The Unicode Standard --
 *                Worldwide Character Encoding -- Version 1.0", Addison-
 *                Wesley, Volume 1, 1991, Volume 2, 1992.  UTF-8 is
 *                described in Unicode Technical Report #4.
 * [US-ASCII]     Coded Character Set--7-bit American Standard Code for
 *                Information Interchange, ANSI X3.4-1986.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 * Original code for IsoLatin1 and UTF-16 by "Martin J. Duerst" <duerst@w3.org>
 */

const static int xmlLittleEndian = 1;
// libxml codes to check if the machine is LittleEndian
// I just hardcode it to 1 here.
// unsigned short int tst = 0x1234;
// unsigned char *ptr = (unsigned char *) &tst;
// if (*ptr == 0x12) xmlLittleEndian = 0;
// else if (*ptr == 0x34) xmlLittleEndian = 1;

/**
 * isolat1ToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ISO Latin 1 chars
 * @inlen:  the length of @in
 *
 * Take a block of ISO Latin 1 chars in and try to convert it to an UTF-8
 * block of chars out.
 * Returns the number of bytes written if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int isolat1ToUTF8(unsigned char *out, int *outlen, const unsigned char *in,
                  int *inlen) {
  unsigned char *outstart = out;
  const unsigned char *base = in;
  unsigned char *outend;
  const unsigned char *inend;
  const unsigned char *instop;

  if ((out == NULL) || (in == NULL) || (outlen == NULL) || (inlen == NULL))
    return (-1);

  outend = out + *outlen;
  inend = in + (*inlen);
  instop = inend;

  while ((in < inend) && (out < outend - 1)) {
    if (*in >= 0x80) {
      *out++ = (((*in) >> 6) & 0x1F) | 0xC0;
      *out++ = ((*in) & 0x3F) | 0x80;
      ++in;
    }
    if ((instop - in) > (outend - out)) instop = in + (outend - out);
    while ((in < instop) && (*in < 0x80)) {
      *out++ = *in++;
    }
  }
  if ((in < inend) && (out < outend) && (*in < 0x80)) {
    *out++ = *in++;
  }
  *outlen = out - outstart;
  *inlen = in - base;
  return (*outlen);
}

/**
 * UTF8Toisolat1:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an ISO Latin 1
 * block of chars out.
 *
 * Returns the number of bytes written if success, -2 if the transcoding fails,
 or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int UTF8Toisolat1(unsigned char *out, int *outlen, const unsigned char *in,
                  int *inlen) {
  const unsigned char *processed = in;
  const unsigned char *outend;
  const unsigned char *outstart = out;
  const unsigned char *instart = in;
  const unsigned char *inend;
  unsigned int c, d;
  int trailing;

  if ((out == NULL) || (outlen == NULL) || (inlen == NULL)) return (-1);
  if (in == NULL) {
    /*
     * initialization nothing to do
     */
    *outlen = 0;
    *inlen = 0;
    return (0);
  }
  inend = in + (*inlen);
  outend = out + (*outlen);
  while (in < inend) {
    d = *in++;
    if (d < 0x80) {
      c = d;
      trailing = 0;
    } else if (d < 0xC0) {
      /* trailing byte in leading position */
      *outlen = out - outstart;
      *inlen = processed - instart;
      return (-2);
    } else if (d < 0xE0) {
      c = d & 0x1F;
      trailing = 1;
    } else if (d < 0xF0) {
      c = d & 0x0F;
      trailing = 2;
    } else if (d < 0xF8) {
      c = d & 0x07;
      trailing = 3;
    } else {
      /* no chance for this in IsoLat1 */
      *outlen = out - outstart;
      *inlen = processed - instart;
      return (-2);
    }

    if (inend - in < trailing) {
      break;
    }

    for (; trailing; trailing--) {
      if (in >= inend) break;
      if (((d = *in++) & 0xC0) != 0x80) {
        *outlen = out - outstart;
        *inlen = processed - instart;
        return (-2);
      }
      c <<= 6;
      c |= d & 0x3F;
    }

    /* assertion: c is a single UTF-4 value */
    if (c <= 0xFF) {
      if (out >= outend) break;
      *out++ = c;
    } else {
      /* no chance for this in IsoLat1 */
      *outlen = out - outstart;
      *inlen = processed - instart;
      return (-2);
    }
    processed = in;
  }
  *outlen = out - outstart;
  *inlen = processed - instart;
  return (*outlen);
}

/**
 * UTF16LEToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @inb:  a pointer to an array of UTF-16LE passwd as a byte array
 * @inlenb:  the length of @in in UTF-16LE chars
 *
 * Take a block of UTF-16LE ushorts in and try to convert it to an UTF-8
 * block of chars out. This function assumes the endian property
 * is the same between the native type of this machine and the
 * inputed one.
 *
 * Returns the number of bytes written, or -1 if lack of space, or -2
 *     if the transcoding fails (if *in is not a valid utf16 string)
 *     The value of *inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 */
int UTF16LEToUTF8(unsigned char *out, int *outlen, const unsigned char *inb,
                  int *inlenb) {
  unsigned char *outstart = out;
  const unsigned char *processed = inb;
  unsigned char *outend = out + *outlen;
  unsigned short *in = (unsigned short *)inb;
  unsigned short *inend;
  unsigned int c, d, inlen;
  unsigned char *tmp;
  int bits;

  if ((*inlenb % 2) == 1) (*inlenb)--;
  inlen = *inlenb / 2;
  inend = in + inlen;
  while ((in < inend) && (out - outstart + 5 < *outlen)) {
    if (xmlLittleEndian) {
      c = *in++;
    } else {
      tmp = (unsigned char *)in;
      c = *tmp++;
      c = c | (((unsigned int)*tmp) << 8);
      in++;
    }
    if ((c & 0xFC00) == 0xD800) { /* high surrogates */
      if (in >= inend) {          /* (in > inend) shouldn't happens */
        /* break; */
        c = 0xfffd; /* output a 'question mark' represents an unknown character
                       */
      } else {
        if (xmlLittleEndian) {
          d = *in++;
        } else {
          tmp = (unsigned char *)in;
          d = *tmp++;
          d = d | (((unsigned int)*tmp) << 8);
          in++;
        }
        if ((d & 0xFC00) == 0xDC00) { /* low surrogates*/
          c &= 0x03FF;
          c <<= 10;
          c |= d & 0x03FF;
          c += 0x10000;
        } else {
          *outlen = out - outstart;
          *inlenb = processed - inb;
          return (-2);
        }
      }
    } else if ((c & 0xFC00) ==
               0xDC00) { /* the first is low surrogates, shouldn't happens*/
      c = 0xfffd; /* output a 'question mark' represents an unknown character */
    }

    /* assertion: c is a single UTF-4 value */
    if (out >= outend) break;
    if (c < 0x80) {
      *out++ = c;
      bits = -6;
    } else if (c < 0x800) {
      *out++ = ((c >> 6) & 0x1F) | 0xC0;
      bits = 0;
    } else if (c < 0x10000) {
      *out++ = ((c >> 12) & 0x0F) | 0xE0;
      bits = 6;
    } else {
      *out++ = ((c >> 18) & 0x07) | 0xF0;
      bits = 12;
    }

    for (; bits >= 0; bits -= 6) {
      if (out >= outend) break;
      *out++ = ((c >> bits) & 0x3F) | 0x80;
    }
    processed = (const unsigned char *)in;
  }
  *outlen = out - outstart;
  *inlenb = processed - inb;
  return (*outlen);
}

/**
 * UTF8ToUTF16LE:
 * @outb:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @outb
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an UTF-16LE
 * block of chars out.
 *
 * Returns the number of bytes written, or -1 if lack of space, or -2
 *     if the transcoding failed.
 */
int UTF8ToUTF16LE(unsigned char *outb, int *outlen, const unsigned char *in,
                  int *inlen) {
  unsigned short *out = (unsigned short *)outb;
  const unsigned char *processed = in;
  const unsigned char *const instart = in;
  unsigned short *outstart = out;
  unsigned short *outend;
  const unsigned char *inend;
  unsigned int c, d;
  int trailing;
  unsigned char *tmp;
  unsigned short tmp1, tmp2;

  /* UTF16LE encoding has no BOM */
  if ((out == NULL) || (outlen == NULL) || (inlen == NULL)) return (-1);
  if (in == NULL) {
    *outlen = 0;
    *inlen = 0;
    return (0);
  }
  inend = in + *inlen;
  outend = out + (*outlen / 2);
  while (in < inend) {
    d = *in++;
    if (d < 0x80) {
      c = d;
      trailing = 0;
    } else if (d < 0xC0) {
      /* trailing byte in leading position */
      *outlen = (out - outstart) * 2;
      *inlen = processed - instart;
      return (-2);
    } else if (d < 0xE0) {
      c = d & 0x1F;
      trailing = 1;
    } else if (d < 0xF0) {
      c = d & 0x0F;
      trailing = 2;
    } else if (d < 0xF8) {
      c = d & 0x07;
      trailing = 3;
    } else {
      /* no chance for this in UTF-16 */
      *outlen = (out - outstart) * 2;
      *inlen = processed - instart;
      return (-2);
    }

    if (inend - in < trailing) {
      break;
    }

    for (; trailing; trailing--) {
      if ((in >= inend) || (((d = *in++) & 0xC0) != 0x80)) break;
      c <<= 6;
      c |= d & 0x3F;
    }

    /* assertion: c is a single UTF-4 value */
    if (c < 0x10000) {
      if (out >= outend) break;
      if (xmlLittleEndian) {
        *out++ = c;
      } else {
        tmp = (unsigned char *)out;
        *tmp = c;
        *(tmp + 1) = c >> 8;
        out++;
      }
    } else if (c < 0x110000) {
      if (out + 1 >= outend) break;
      c -= 0x10000;
      if (xmlLittleEndian) {
        *out++ = 0xD800 | (c >> 10);
        *out++ = 0xDC00 | (c & 0x03FF);
      } else {
        tmp1 = 0xD800 | (c >> 10);
        tmp = (unsigned char *)out;
        *tmp = (unsigned char)tmp1;
        *(tmp + 1) = tmp1 >> 8;
        out++;

        tmp2 = 0xDC00 | (c & 0x03FF);
        tmp = (unsigned char *)out;
        *tmp = (unsigned char)tmp2;
        *(tmp + 1) = tmp2 >> 8;
        out++;
      }
    } else
      break;
    processed = in;
  }
  *outlen = (out - outstart) * 2;
  *inlen = processed - instart;
  return (*outlen);
}

/**
 * UTF8ToUTF16BE:
 * @outb:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @outb
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an UTF-16BE
 * block of chars out.
 *
 * Returns the number of byte written, or -1 by lack of space, or -2
 *     if the transcoding failed.
 */
int UTF8ToUTF16BE(unsigned char *outb, int *outlen, const unsigned char *in,
                  int *inlen) {
  unsigned short *out = (unsigned short *)outb;
  const unsigned char *processed = in;
  const unsigned char *const instart = in;
  unsigned short *outstart = out;
  unsigned short *outend;
  const unsigned char *inend;
  unsigned int c, d;
  int trailing;
  unsigned char *tmp;
  unsigned short tmp1, tmp2;

  /* UTF-16BE has no BOM */
  if ((outb == NULL) || (outlen == NULL) || (inlen == NULL)) return (-1);
  if (in == NULL) {
    *outlen = 0;
    *inlen = 0;
    return (0);
  }
  inend = in + *inlen;
  outend = out + (*outlen / 2);
  while (in < inend) {
    d = *in++;
    if (d < 0x80) {
      c = d;
      trailing = 0;
    } else if (d < 0xC0) {
      /* trailing byte in leading position */
      *outlen = out - outstart;
      *inlen = processed - instart;
      return (-2);
    } else if (d < 0xE0) {
      c = d & 0x1F;
      trailing = 1;
    } else if (d < 0xF0) {
      c = d & 0x0F;
      trailing = 2;
    } else if (d < 0xF8) {
      c = d & 0x07;
      trailing = 3;
    } else {
      /* no chance for this in UTF-16 */
      *outlen = out - outstart;
      *inlen = processed - instart;
      return (-2);
    }

    if (inend - in < trailing) {
      break;
    }

    for (; trailing; trailing--) {
      if ((in >= inend) || (((d = *in++) & 0xC0) != 0x80)) break;
      c <<= 6;
      c |= d & 0x3F;
    }

    /* assertion: c is a single UTF-4 value */
    if (c < 0x10000) {
      if (out >= outend) break;
      if (xmlLittleEndian) {
        tmp = (unsigned char *)out;
        *tmp = c >> 8;
        *(tmp + 1) = c;
        out++;
      } else {
        *out++ = c;
      }
    } else if (c < 0x110000) {
      if (out + 1 >= outend) break;
      c -= 0x10000;
      if (xmlLittleEndian) {
        tmp1 = 0xD800 | (c >> 10);
        tmp = (unsigned char *)out;
        *tmp = tmp1 >> 8;
        *(tmp + 1) = (unsigned char)tmp1;
        out++;

        tmp2 = 0xDC00 | (c & 0x03FF);
        tmp = (unsigned char *)out;
        *tmp = tmp2 >> 8;
        *(tmp + 1) = (unsigned char)tmp2;
        out++;
      } else {
        *out++ = 0xD800 | (c >> 10);
        *out++ = 0xDC00 | (c & 0x03FF);
      }
    } else
      break;
    processed = in;
  }
  *outlen = (out - outstart) * 2;
  *inlen = processed - instart;
  return (*outlen);
}

/**
 * UTF16BEToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @inb:  a pointer to an array of UTF-16 passed as a byte array
 * @inlenb:  the length of @in in UTF-16 chars
 *
 * Take a block of UTF-16 ushorts in and try to convert it to an UTF-8
 * block of chars out. This function assumes the endian property
 * is the same between the native type of this machine and the
 * inputed one.
 *
 * Returns the number of bytes written, or -1 if lack of space, or -2
 *     if the transcoding fails (if *in is not a valid utf16 string)
 * The value of *inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 */
int UTF16BEToUTF8(unsigned char *out, int *outlen, const unsigned char *inb,
                  int *inlenb) {
  unsigned char *outstart = out;
  const unsigned char *processed = inb;
  unsigned char *outend = out + *outlen;
  unsigned short *in = (unsigned short *)inb;
  unsigned short *inend;
  unsigned int c, d, inlen;
  unsigned char *tmp;
  int bits;

  if ((*inlenb % 2) == 1) (*inlenb)--;
  inlen = *inlenb / 2;
  inend = in + inlen;
  while (in < inend) {
    if (xmlLittleEndian) {
      tmp = (unsigned char *)in;
      c = *tmp++;
      c = c << 8;
      c = c | (unsigned int)*tmp;
      in++;
    } else {
      c = *in++;
    }
    if ((c & 0xFC00) == 0xD800) { /* high surrogates */
      if (in >= inend) {          /* (in > inend) shouldn't happens */
        /*
        *outlen = out - outstart;
        *inlenb = processed - inb;
        return (-2);
        */
        c = 0xfffd; /* output a 'question mark' represents an unknown character
                       */
      } else {
        if (xmlLittleEndian) {
          tmp = (unsigned char *)in;
          d = *tmp++;
          d = d << 8;
          d = d | (unsigned int)*tmp;
          in++;
        } else {
          d = *in++;
        }
        if ((d & 0xFC00) == 0xDC00) { /* low surrogates */
          c &= 0x03FF;
          c <<= 10;
          c |= d & 0x03FF;
          c += 0x10000;
        } else {
          *outlen = out - outstart;
          *inlenb = processed - inb;
          return (-2);
        }
      }
    } else if ((c & 0xFC00) ==
               0xDC00) { /* the first is low surrogates, shouldn't happens*/
      c = 0xfffd; /* output a 'question mark' represents an unknown character */
    }

    /* assertion: c is a single UTF-4 value */
    if (out >= outend) break;
    if (c < 0x80) {
      *out++ = c;
      bits = -6;
    } else if (c < 0x800) {
      *out++ = ((c >> 6) & 0x1F) | 0xC0;
      bits = 0;
    } else if (c < 0x10000) {
      *out++ = ((c >> 12) & 0x0F) | 0xE0;
      bits = 6;
    } else {
      *out++ = ((c >> 18) & 0x07) | 0xF0;
      bits = 12;
    }

    for (; bits >= 0; bits -= 6) {
      if (out >= outend) break;
      *out++ = ((c >> bits) & 0x3F) | 0x80;
    }
    processed = (const unsigned char *)in;
  }
  *outlen = out - outstart;
  *inlenb = processed - inb;
  return (*outlen);
}


size_t charset_convert(const char *to_charset, const char *from_charset,
                       char **inbuf, size_t *inbytesleft, char **outbuf,
                       size_t *outbytesleft) {
  size_t buf_len = *outbytesleft;
  iconv_t cd = iconv_open(to_charset, from_charset);
  if (cd == (iconv_t)-1) {
    LOG_ERROR("charset is not supportedi. Failed to convert charset from "
              << from_charset << " to " << to_charset);
    return -1;
  }
  size_t num_nonreversible_or_error = iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft);
  if (num_nonreversible_or_error < 0 || *inbytesleft != 0) {
    LOG_ERROR("Failed to convert charset from "
                  << from_charset << " to " << to_charset << ". inbytesleft="
                  << *inbytesleft << ", outbytesleft=" << *outbytesleft
              << ", iconv() return errno=" << errno);
  }
  iconv_close(cd);

  return (buf_len - *outbytesleft);
}

}  // namespace encoding
}  // namespace ming
