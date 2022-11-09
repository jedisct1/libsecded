CC=gcc -fPIC -O3 -march=native -mtune=native -Wall -Wextra

CSRC=expand.c parity.c fill.c correct.c shrink.c encode.c clean.c decode.c

all: test.out test2.out

test2.out: test2.py libsecded.so
	./test2.py > test2.out
	cmp test2.out test2.exp

test.out: test
	env LD_LIBRARY_PATH=. ./test > test.out
	cmp test.out test.exp

test: test.c libsecded.so
	$(CC) -o test test.c libsecded.so

COBJ=$(CSRC:.c=.o)
CDEP=$(CSRC:.c=.d)

libsecded.so: libsecded.so.1
	rm -f $@
	cp $< $@

libsecded.so.1: $(COBJ)
	$(CC) -shared -o $@ -Wl,-soname,$@ $^

$(COBJ): %.o: %.c
	$(CC) -MMD -MP -c $<

.SECONDARY:

-include $(CDEP)
