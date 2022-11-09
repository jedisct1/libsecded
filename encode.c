#include <inttypes.h>
#include "secded.h"

void secded_encode(void *x,long long *n)
{
  secded_expand(x,n);
  if (x) {
    uint8_t parity[64];
    secded_parity(parity,x,*n);
    secded_fill(x,*n,parity);
  }
}
