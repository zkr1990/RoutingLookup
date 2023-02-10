// Copyright (c) 2011 Google, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// CityHash, by Geoff Pike and Jyrki Alakuijala
//
// This file provides CityHash64() and related functions.
//
// It's probably possible to create even faster hash functions by
// writing a program that systematically explores some of the space of
// possible hash functions, by using SIMD instructions, or by
// compromising on hash quality.
//
// Comment by LHZ 2017.5.19:
// This hash function has been tested by the following dataset:
// 1. 2G (items) Chinese word groups from 40G Chinese news text
// 2. 4M Chinese words from 40G Chinese news text
// 3. 1M Chinese words dictionary
// No conflict found yet.

#include "City64.h"
#include "Type64.h"
#include <string.h>

#define Fetch64(x) (*(uint64_t*)(x))
#define Fetch32(x) (*(uint32_t*)(x))

enum
{
// Some primes between 2^63 and 2^64 for various uses.
    k0 = 0xc3a5c85c97cb3127ULL,
    k1 = 0xb492b66fbe98f273ULL,
    k2 = 0x9ae16a3b2f90404fULL,
    k3 = 0xc949d7c7509e6557ULL,
// for Hash128to64
    kMul = 0x9ddfea08eb382d69ULL,
};
// Hash 128 input bits down to 64 bits of output.
// This is intended to be a reasonably good hash function.
inline uint64_t Hash128to64(uint64_t vL, uint64_t vH) {
  // Murmur-inspired hashing.
  uint64_t a = (vL ^ vH) * kMul;
  a ^= (a >> 47);
  uint64_t b = (vH ^ a) * kMul;
  b ^= (b >> 47);
  b *= kMul;
  return b;
}

// Bitwise right rotate.  Normally this will compile to a single
// instruction, especially if the shift is a manifest constant.
inline uint64_t Rotate(uint64_t val, int shift) {
  // Avoid shifting by 64: doing so yields an undefined result.
  return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
}

// Equivalent to Rotate(), but requires the second arg to be non-zero.
// On x86-64, and probably others, it's possible for this to compile
// to a single instruction if both args are already in registers.
inline uint64_t RotateByAtLeast1(uint64_t val, int shift) {
  return (val >> shift) | (val << (64 - shift));
}

inline uint64_t ShiftMix(uint64_t val) {
  return val ^ (val >> 47);
}

#define HashLen16 Hash128to64

inline uint64_t HashLen0to16(const char *s, size_t len) {
  if (len > 8) {
    uint64_t a = Fetch64(s);
    uint64_t b = Fetch64(s + len - 8);
    return HashLen16(a, RotateByAtLeast1(b + len, len)) ^ b;
  }
  if (len >= 4) {
    uint64_t a = Fetch32(s);
    return HashLen16(len + (a << 3), Fetch32(s + len - 4));
  }
  if (len > 0) {
    uint32_t y = static_cast<uint32_t>(s[0]) + (static_cast<uint32_t>(s[len >> 1]) << 8);
    uint32_t z = len + (static_cast<uint32_t>(s[len - 1]) << 2);

    return ShiftMix(y * k2 ^ z * k3) * k2;
  }
  return k2;
}
/*
inline uint64_t HashLen0to16(const char *s, size_t len) {
    static uint8_t off[]={
        3,
        2,2,2,
        1,1,1,1,1,
        0,0,0,0,0,0,0,0};

    switch(off[len])
    {
  case 0:
  {
    uint64_t a = Fetch64(s);
    uint64_t b = Fetch64(s + len - 8);
    return HashLen16(a, RotateByAtLeast1(b + len, len)) ^ b;
  }
  case 1:
  {
    uint64_t a = Fetch32(s);
    return HashLen16(len + (a << 3), Fetch32(s + len - 4));
  }
  case 2:
  {
    uint32_t y = static_cast<uint32_t>(s[0]) + (static_cast<uint32_t>(s[len >> 1]) << 8);
    uint32_t z = len + (static_cast<uint32_t>(s[len - 1]) << 2);

    return ShiftMix(y * k2 ^ z * k3) * k2;
  }
  case 3:
    return k2;
    }
}
*/

// This probably works well for 16-byte strings as well, but it may be overkill
// in that case.
inline uint64_t HashLen17to32(const char *s, size_t len) {
  uint64_t a = Fetch64(s) * k1;
  uint64_t b = Fetch64(s + 8);
  uint64_t c = Fetch64(s + len - 8) * k2;
  uint64_t d = Fetch64(s + len - 16) * k0;
  return HashLen16(Rotate(a - b, 43) + Rotate(c, 30) + d,
                   a + Rotate(b ^ k3, 20) - c + len);
}

// Return a 16-byte hash for 48 bytes.  Quick and dirty.
// Callers do best to use "random-looking" values for a and b.
inline void WeakHashLen32WithSeeds(TX128Data &out,
    uint64_t w, uint64_t x, uint64_t y, uint64_t z, uint64_t a, uint64_t b) {
  a += w;
  b = Rotate(b + a + z, 21);
  uint64_t c = a;
  a += x;
  a += y;
  b += Rotate(a, 44);
  out.u64[0] = a+z;
  out.u64[1] = b+c;
}

// Return a 16-byte hash for s[0] ... s[31], a, and b.  Quick and dirty.
inline void WeakHashLen32WithSeeds(TX128Data &out,
    const char* s, uint64_t a, uint64_t b) {
  WeakHashLen32WithSeeds(out,Fetch64(s),
                                Fetch64(s + 8),
                                Fetch64(s + 16),
                                Fetch64(s + 24),
                                a,
                                b);
}

// Return an 8-byte hash for 33 to 64 bytes.
inline uint64_t HashLen33to64(const char *s, size_t len) {
  uint64_t z = Fetch64(s + 24);
  uint64_t a = Fetch64(s) + (len + Fetch64(s + len - 16)) * k0;
  uint64_t b = Rotate(a + z, 52);
  uint64_t c = Rotate(a, 37);
  a += Fetch64(s + 8);
  c += Rotate(a, 7);
  a += Fetch64(s + 16);
  uint64_t vf = a + z;
  uint64_t vs = b + Rotate(a, 31) + c;
  a = Fetch64(s + 16) + Fetch64(s + len - 32);
  z = Fetch64(s + len - 8);
  b = Rotate(a + z, 52);
  c = Rotate(a, 37);
  a += Fetch64(s + len - 24);
  c += Rotate(a, 7);
  a += Fetch64(s + len - 16);
  uint64_t wf = a + z;
  uint64_t ws = b + Rotate(a, 31) + c;
  uint64_t r = ShiftMix((vf + ws) * k2 + (wf + vs) * k0);
  return ShiftMix(r * k0 + vs) * k2;
}

uint64_t CityHash64(const char *s, size_t len) {
  if (len <= 32) {
    if (len <= 16) {
      return HashLen0to16(s, len);
    } else {
      return HashLen17to32(s, len);
    }
  } else if (len <= 64) {
    return HashLen33to64(s, len);
  }

  // For strings over 64 bytes we hash the end first, and then as we
  // loop we keep 56 bytes of state: v, w, x, y, and z.
  uint64_t x = Fetch64(s + len - 40);
  uint64_t y = Fetch64(s + len - 16) + Fetch64(s + len - 56);
  uint64_t z = HashLen16(Fetch64(s + len - 48) + len, Fetch64(s + len - 24));
  TX128Data v,w;

  WeakHashLen32WithSeeds(v, s + len - 64, len, z);
  WeakHashLen32WithSeeds(w, s + len - 32, y + k1, x);
  x = x * k1 + Fetch64(s);

  // Decrease len to the nearest multiple of 64, and operate on 64-byte chunks.
  len = (len - 1) & ~static_cast<size_t>(63);
  do {
    x = Rotate(x + y + v.u64[0] + Fetch64(s + 8), 37) * k1;
    y = Rotate(y + v.u64[1] + Fetch64(s + 48), 42) * k1;
    x ^= w.u64[1];
    y += v.u64[0] + Fetch64(s + 40);
    z = Rotate(z + w.u64[0], 33) * k1;
    WeakHashLen32WithSeeds(v, s, v.u64[1] * k1, x + w.u64[0]);
    WeakHashLen32WithSeeds(w, s + 32, z + w.u64[1], y + Fetch64(s + 16));
    uint64_t tmp=z;z=x;x=tmp;
    s += 64;
    len -= 64;
  } while (len != 0);
  return HashLen16(HashLen16(v.u64[0], w.u64[0]) + ShiftMix(y) * k1 + z,
                   HashLen16(v.u64[1], w.u64[1]) + x);
}


uint64_t CityHash64WithSeeds(const char *s, size_t len,
                           uint64_t seed0, uint64_t seed1) {
  return HashLen16(CityHash64(s, len) - seed0, seed1);
}

uint64_t CityHash64WithSeed(const char *s, size_t len, uint64_t seed) {
  return CityHash64WithSeeds(s, len, k2, seed);
}

uint64_t FastStrHash(const char *str,int len)
{
	if(len<0)
		len = strlen(str);

	return CityHash64(str,len);
}

uint64_t FastIntHash(uint64_t hash,uint64_t seed)
{
  return HashLen16(hash - k2, seed);
}
