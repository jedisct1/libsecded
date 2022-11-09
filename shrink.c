#include <string.h>
#include <inttypes.h>
#include "secded.h"

void secded_shrink(void *x_arg,long long *n_arg)
{
  uint8_t *x = (uint8_t *) x_arg;
  long long n = *n_arg;
  long long bits = 0;
  long long twobits = 1;

  if (n <= 3) { *n_arg = 0; return; }
  while (twobits < n) {
    ++bits;
    twobits += twobits;
  }
  /* now twobits >= n >= 4 */
  *n_arg = n-bits-1;

  if (x) {
    long long bit = 1;
    long long twobit = 2;
    while (bit < bits-1) {
      memmove(x+twobit-1-bit,x+twobit+1,twobit-1);
      ++bit;
      twobit += twobit;
    }
    memmove(x+twobit-1-bit,x+twobit+1,n-twobit-1);
  }
}
