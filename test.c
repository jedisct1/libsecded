#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include "secded.h"

#define N 65536
#define pad 128
/* pad must be at least 64 for the secded api */
/* and is taken larger here to check for overwrites */

uint8_t m[N+pad];
uint8_t c[N+pad]; long long clen;
uint8_t r[N+pad]; long long rlen;

int main()
{
  unsigned int secded;
  long long expectsec,expectded;

  for (long long mlen = 0;mlen <= N;++mlen) {
    long long padlen = mlen+pad;

    for (long long loop = 0;loop < 100;++loop) {
      for (long long j = 0;j < padlen;++j) m[j] = random();

      for (long long j = 0;j < padlen;++j) c[j] = m[j];
      clen = mlen;
      secded_encode(c,&clen);
      assert(clen >= mlen);
      assert(clen <= mlen+64);
      for (long long j = clen;j < padlen;++j) assert(c[j] == m[j]);

      /* just sizes */
      rlen = mlen;
      secded_encode(0,&rlen);
      assert(rlen == clen);
      secded_decode(0,&rlen);
      assert(rlen == mlen);
      secded_expand(0,&rlen);
      assert(rlen == clen);
      secded_shrink(0,&rlen);
      assert(rlen == mlen);

      /* no errors, decoding */
      for (long long j = 0;j < padlen;++j) r[j] = c[j];
      rlen = clen;
      secded = secded_decode(r,&rlen);
      assert(rlen == mlen);
      assert(secded == 0);
      for (long long j = 0;j < mlen;++j) assert(r[j] == m[j]);
      for (long long j = clen;j < padlen;++j) assert(r[j] == m[j]);

      /* no errors, cleaning */
      for (long long j = 0;j < padlen;++j) r[j] = c[j];
      rlen = clen;
      secded = secded_clean(r,rlen);
      for (long long j = 0;j < clen;++j) assert(r[j] == c[j]);
      for (long long j = clen;j < padlen;++j) assert(r[j] == m[j]);

      /* invalid lengths */
      for (long long xlen = clen+1;xlen <= padlen;++xlen) {
        if ((xlen-1)&(xlen-2)) break;
        for (long long j = 0;j < padlen;++j) r[j] = c[j];
        rlen = xlen;
        secded_decode(r,&rlen);
        assert(rlen == mlen);
        for (long long j = 0;j < mlen;++j) assert(r[j] == m[j]);
      }

      /* one error, decoding */
      if (rlen >= 1) {
        for (long long j = 0;j < padlen;++j) r[j] = c[j];
        rlen = clen;
        expectsec = 1+(random()%255);
        for (long long bit = 0;bit < 8;++bit)
          if (expectsec & (1<<bit))
            r[random()%rlen] ^= 1<<bit;
        secded = secded_decode(r,&rlen);
        assert(rlen == mlen);
        assert(secded == expectsec);
        for (long long j = 0;j < mlen;++j) assert(r[j] == m[j]);
        for (long long j = clen;j < padlen;++j) assert(r[j] == m[j]);
      }

      /* one error, cleaning */
      if (rlen >= 1) {
        for (long long j = 0;j < padlen;++j) r[j] = c[j];
        rlen = clen;
        expectsec = 1+(random()%255);
        for (long long bit = 0;bit < 8;++bit)
          if (expectsec & (1<<bit))
            r[random()%rlen] ^= 1<<bit;
        secded = secded_clean(r,rlen);
        assert(rlen == clen);
        assert(secded == expectsec);
        for (long long j = 0;j < clen;++j) assert(r[j] == c[j]);
        for (long long j = clen;j < padlen;++j) assert(r[j] == m[j]);
      }

      /* two errors */
      if (rlen >= 2) {
        for (long long j = 0;j < padlen;++j) r[j] = c[j];
        rlen = clen;
        expectded = 1+(random()%255);
        for (long long bit = 0;bit < 8;++bit)
          if (expectded & (1<<bit)) {
            long long e1 = random()%rlen;
            long long e2 = random()%rlen;
            while (e2 == e1) e2 = random()%rlen;
            r[e1] ^= 1<<bit;
            r[e2] ^= 1<<bit;
          }
        secded = secded_decode(r,&rlen);
        assert(rlen == mlen);
        assert(secded == expectded*256);
        for (long long j = clen;j < padlen;++j) assert(r[j] == m[j]);
      }

      /* mix of one and two errors */
      if (rlen >= 2) {
        for (long long j = 0;j < padlen;++j) r[j] = c[j];
        rlen = clen;
        expectsec = 1+(random()%255);
        expectded = 1+(random()%255);
        expectded &= ~expectsec;
        for (long long bit = 0;bit < 8;++bit) {
          if (expectsec & (1<<bit))
            r[random()%rlen] ^= 1<<bit;
          if (expectded & (1<<bit)) {
            long long e1 = random()%rlen;
            long long e2 = random()%rlen;
            while (e2 == e1) e2 = random()%rlen;
            r[e1] ^= 1<<bit;
            r[e2] ^= 1<<bit;
          }
        }
        secded = secded_decode(r,&rlen);
        assert(rlen == mlen);
        assert(secded == expectsec+expectded*256);
        for (long long j = clen;j < padlen;++j) assert(r[j] == m[j]);
      }
    }

    printf("bytes %lld encoded %lld",mlen,clen);
    for (long long xlen = clen+1;xlen <= padlen;++xlen) {
      if ((xlen-1)&(xlen-2)) break;
      printf(" %lld",xlen);
    }
    printf("\n");
    fflush(stdout);

    if (mlen > 1024) mlen += mlen/16;
  }

  return 0;
}
