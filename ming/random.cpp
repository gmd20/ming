// xorshift128+  Math.random() from V8 JavaScript Engine
// http://v8project.blogspot.de/2015/12/theres-mathrandom-and-then-theres.html
// http://vigna.di.unimi.it/ftp/papers/xorshiftplus.pdf  Further scramblings of Marsaglia’s xorshift generators


uint64_t state0 = 1;
uint64_t state1 = 2;
uint64_t xorshift128plus() {
  uint64_t s1 = state0;
  uint64_t s0 = state1;
  state0 = s0;
  s1 ^= s1 << 23;
  s1 ^= s1 >> 17;
  s1 ^= s0;
  s1 ^= s0 >> 26;
  state1 = s1;
  return state0 + state1;
}
