#ifndef secded_h
#define secded_h

/* API: */

void secded_encode(void *,long long *);
unsigned int secded_clean(void *,long long);
unsigned int secded_decode(void *,long long *);

/* internals: */

void secded_expand(void *,long long *);
void secded_parity(void *,const void *,long long);
void secded_fill(void *,long long,const void *);
unsigned int secded_correct(void *,long long,const void *);
void secded_shrink(void *,long long *);

#endif
