#include <string.h>
#include <inttypes.h>
#include "secded.h"

void secded_expand(void *x_arg,long long *n_arg)
{
  uint8_t *x = (uint8_t *) x_arg;
  long long n = *n_arg;
  long long bits = 0;
  long long twobits = 1;

  if (n <= 0) return;
  n += 1;
  while (twobits < n) {
    ++n;
    ++bits;
    twobits += twobits;
  }
  /* now twobits >= n >= 4 */

  *n_arg = n;
  if (x) {
    long long bit = bits-1;
    long long twobit = twobits>>1;

    memmove(x+twobit+1,x+twobit-1-bit,n-twobit-1);
    x[twobit] = 0;
    while (bit > 1) {
      --bit;
      twobit >>= 1;
      memmove(x+twobit+1,x+twobit-1-bit,twobit-1);
      x[twobit] = 0;
    }
    x[0] = 0;
    x[1] = 0;
  }
}
