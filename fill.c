#include <inttypes.h>
#include "secded.h"

void secded_fill(void *x_arg,long long n,const void *parity_arg)
{
  long long bit = 0;
  long long twobit = 1;
  uint8_t p,overall = 0;
  uint8_t *x = (uint8_t *) x_arg;
  const uint8_t *parity = (const uint8_t *) parity_arg;
  if (n <= 0) return;
  while (twobit < n) {
    p = parity[bit];
    overall ^= p;
    x[twobit] = p;
    ++bit;
    twobit += twobit;
  }
  x[0] = overall^parity[bit];
}
