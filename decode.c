#include <inttypes.h>
#include "secded.h"

unsigned int secded_decode(void *x,long long *n)
{
  unsigned int secded = 0;
  if (x) secded = secded_clean(x,*n);
  secded_shrink(x,n);
  return secded;
}
