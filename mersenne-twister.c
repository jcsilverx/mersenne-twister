/**
 *  Mersenne Twister
 * 
 *  jcsilverx
 */
#include <stdint.h>
#include <stdlib.h>
#include <emscripten.h>

#define N       624
#define M       397
#define W       32
#define R       31
#define UMASK   (0xFFFFFFFFUL << R)
#define LMASK   (0xFFFFFFFFUL >> (W - R))
#define A       0x9908B0DFUL
#define U       11
#define S       7
#define T       15
#define L       18
#define B       0x9D2C5680UL
#define C       0xEFC60000UL
#define F       1812433253UL

typedef struct {
  uint32_t state_array[N];
  int state_index;
} mt_state;

void
initialize_state(mt_state *state, uint32_t seed)
{
  int i;
  uint32_t *state_array = &(state->state_array[0]);

  state_array[0] = seed;

  i = 1;
  while (i < N) {
    seed = F * (seed ^ (seed >> (W - 2))) + i;  // Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.

    state_array[i] = seed;

    ++i;
  }

  state->state_index = 0;
}

uint32_t
random_uint32(mt_state *state)
{
  uint32_t *state_array = &(state->state_array[0]);
  int k = state->state_index;

  int j = k - (N - 1);
  if (j < 0) j += N;

  uint32_t x = (state_array[k] & UMASK) | (state_array[j] & LMASK);

  uint32_t xA = x >> 1;
  if (x & 0x00000001UL) xA ^= A;

  j = k - (N - M);
  if (j < 0) j += N;

  x = state_array[j] ^ xA;
  state_array[k++] = x;

  if (k >= N) k = 0;
  state->state_index = k;

  uint32_t y = x ^ (x >> U);
  
  y = y ^ ((y << S) & B);
  y = y ^ ((y << T) & C);
  
  uint32_t z = y ^ (y >> L);

  return z;
}

/**
 * Exposed Functions for WASM
 */
EMSCRIPTEN_KEEPALIVE
mt_state *
create_state(uint32_t seed)
{
  mt_state *state = (mt_state *)malloc(sizeof(mt_state));
  
  initialize_state(state, seed);

  return state;
}

EMSCRIPTEN_KEEPALIVE
void
destroy_state(mt_state *state)
{
  free(state);
}

EMSCRIPTEN_KEEPALIVE
uint32_t
generate(mt_state *state)
{
  return random_uint32(state);
}