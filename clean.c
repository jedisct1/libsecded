#include <inttypes.h>
#include "secded.h"

unsigned int secded_clean(void *x,long long n)
{
  uint8_t parity[64];
  secded_parity(parity,x,n);
  return secded_correct(x,n,parity);
}
