#ifndef RC4_RANDOM_H_
#define RC4_RANDOM_H_

static inline void rc4_swap(uint8_t *a, uint8_t *b) {
  uint8_t t;
  t = *a;
  *a = *b;
  *b = t;
}

struct rc4_ctx {
  uint8_t s[256];
  int x, y;
};

void rc4_init(struct rc4_ctx *ctx, void *key, int key_len) {
  int i, j;
  ctx->x = ctx->y = j = 0;
  for (i = 0; i < 256; i ++) {
    ctx->s[i] = i;
  }
  for (i = 0; i < 256; i ++) {
    j = (j + ctx->s[i] + ((uint8_t *) key)[i % key_len]) & 255;
    rc4_swap(&ctx->s[i], &ctx->s[j]);
  }
}

uint8_t rc4_random_uint8(struct rc4_ctx *ctx) {
  ctx->x = (ctx->x + 1) & 255;
  ctx->y = (ctx->y + ctx->s[ctx->x]) & 255;
  rc4_swap(&ctx->s[ctx->x], &ctx->s[ctx->y]);
  return ctx->s[(ctx->s[ctx->x] + ctx->s[ctx->y]) & 255];
}

uint32_t rc4_random_uint32(struct rc4_ctx *ctx) {
  uint32_t i = rc4_random_uint8(ctx);
  i <<= 8;
  i |= rc4_random_uint8(ctx);
  i <<= 8;
  i |= rc4_random_uint8(ctx);
  i <<= 8;
  i |= rc4_random_uint8(ctx);
  return i;
}

#endif /* RC4_RANDOM_H_ */
