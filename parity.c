#include <inttypes.h>
#include "secded.h"

#define BOTTOM 8
#define TWOBOTTOM 256
#define TWOPART 32

void secded_parity(void *parity_arg,const void *x_arg,long long n)
{
  const uint8_t *x = (const uint8_t *) x_arg;
  uint8_t *parity = (uint8_t *) parity_arg;
  uint8_t sum;
  long long bits = 0;
  long long twobits = 1;
  long long bit = 0;
  long long twobit = 1;
  long long ntop = n>>BOTTOM;
  uint8_t prexor[TWOBOTTOM];

  while (twobits < n) {
    ++bits;
    twobits += twobits;
  }

  for (long long j = 0;j < TWOBOTTOM;++j)
    prexor[j] = 0;
  for (long long j = 0;j < ntop;++j)
    for (long long k = 0;k < TWOBOTTOM;++k)
      prexor[k] ^= x[j*TWOBOTTOM+k];
  for (long long j = ntop*TWOBOTTOM;j < n;++j)
    prexor[j%TWOBOTTOM] ^= x[j];

  sum = 0;
  for (long long j = 0;j < TWOBOTTOM;++j)
    sum ^= prexor[j];
  parity[bits] = sum;

  while (twobit < n && twobit < TWOBOTTOM) {
    sum = 0;
    for (long long j = 0;j < TWOBOTTOM;++j)
      if (j & twobit)
        sum ^= prexor[j];
    parity[bit] = sum;
    ++bit;
    twobit += twobit;
  }

  for (bit = BOTTOM;bit < bits;++bit)
    parity[bit] = 0;

  for (long long j = 0;j < ntop;++j) {
    for (long long k = 0;k < TWOPART;++k)
      prexor[k] = x[j*TWOBOTTOM+k];
    for (long long h = TWOPART;h < TWOBOTTOM;h += TWOPART)
      for (long long k = 0;k < TWOPART;++k)
        prexor[k] ^= x[j*TWOBOTTOM+h+k];
    sum = 0;
    for (long long k = 0;k < TWOPART;++k)
      sum ^= prexor[k];
    for (bit = BOTTOM;bit < bits;++bit)
      if (j&(1<<(bit-BOTTOM)))
        parity[bit] ^= sum;
  }
  for (long long j = ntop;j < ntop+1;++j) {
    sum = 0;
    for (long long k = 0;k < n-j*TWOBOTTOM;++k)
      sum ^= x[j*TWOBOTTOM+k];
    for (bit = BOTTOM;bit < bits;++bit)
      if (j&(1<<(bit-BOTTOM)))
        parity[bit] ^= sum;
  }
}
