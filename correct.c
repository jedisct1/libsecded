#include <inttypes.h>
#include "secded.h"

#define BOTTOM 7
#define TWOBOTTOM 128

unsigned int secded_correct(void *x_arg,long long n,const void *parity_arg)
{
  uint8_t *x = (uint8_t *) x_arg;
  const uint8_t *parity = (const uint8_t *) parity_arg;
  long long bits = 0;
  long long twobits = 1;
  long long bit,twobit;
  uint8_t flipbottom[TWOBOTTOM];
  long long ntop = n>>BOTTOM;
  uint8_t flip,fliptop,overall,anyp,borrow,nmask,pmask,sec,ded;

  if (n <= 0) return 0;
  while (twobits < n) {
    ++bits;
    twobits += twobits;
  }

  overall = parity[bits];

  anyp = 0;
  borrow = 0;
  for (bit = 0;bit < bits;++bit) {
    nmask = -(1&(n>>bit));
    pmask = parity[bit];
    anyp |= pmask;
    borrow = (nmask&borrow&pmask)|((nmask|borrow)&~pmask);
  }
  nmask = -(1&(n>>bits));
  borrow |= nmask;

  sec = borrow&overall;
  ded = (anyp&~overall)|(overall&~borrow);

  for (long long j = 0;j < TWOBOTTOM;++j) {
    flip = 0xff;
    bit = 0;
    twobit = 1;
    while (bit < bits && bit < BOTTOM) {
      if (j & twobit)
        flip &= parity[bit];
      else
        flip &= ~parity[bit];
      ++bit;
      twobit += twobit;
    }
    flipbottom[j] = flip;
  }

  for (long long k = 0;k < ntop<<BOTTOM;k += TWOBOTTOM) {
    fliptop = overall;
    bit = BOTTOM;
    twobit = TWOBOTTOM;
    while (bit < bits) {
      if (k & twobit)
        fliptop &= parity[bit];
      else
        fliptop &= ~parity[bit];
      ++bit;
      twobit += twobit;
    }
    
    for (long long j = 0;j < TWOBOTTOM;++j)
      x[k+j] ^= fliptop&flipbottom[j];
  }

  {
    long long k = ntop<<BOTTOM;
    fliptop = overall;
    bit = BOTTOM;
    twobit = TWOBOTTOM;
    while (bit < bits) {
      if (k & twobit)
        fliptop &= parity[bit];
      else
        fliptop &= ~parity[bit];
      ++bit;
      twobit += twobit;
    }
    
    for (long long j = 0;j < TWOBOTTOM;++j)
      if (k+j < n)
        x[k+j] ^= fliptop&flipbottom[j];
  }

  return ((unsigned int) sec) | (((unsigned int) ded) << 8);
}
