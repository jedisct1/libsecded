#!/usr/bin/env python3

import os
from random import randrange
import sys
from ctypes import CDLL,byref,POINTER,c_char,c_uint,c_longlong,create_string_buffer

def randbyte():
  return randrange(256)

def randerrpos():
  return 1+randrange(255)

libsecded = CDLL('%s/libsecded.so.1' % os.getcwd())
libsecded.secded_encode.argtypes = POINTER(c_char),POINTER(c_longlong)
libsecded.secded_clean.argtypes = POINTER(c_char),c_longlong
libsecded.secded_clean.restype = c_uint
libsecded.secded_decode.argtypes = POINTER(c_char),POINTER(c_longlong)
libsecded.secded_decode.restype = c_uint

libsecded.secded_expand.argtypes = POINTER(c_char),POINTER(c_longlong)
libsecded.secded_parity.argtypes = POINTER(c_char),POINTER(c_char),c_longlong
libsecded.secded_fill.argtypes = POINTER(c_char),c_longlong,POINTER(c_char)
libsecded.secded_correct.argtypes = POINTER(c_char),c_longlong,POINTER(c_char)
libsecded.secded_correct.restype = c_uint
libsecded.secded_shrink.argtypes = POINTER(c_char),POINTER(c_longlong)

from encode import encode
from clean import clean
from decode import decode

from expand import expand
from parity import parity
from fill import fill
from correct import correct
from shrink import shrink

for n in range(32):
  print(n)
  sys.stdout.flush()

  for loop in range(100):
    m = [randbyte() for j in range(n)]
    x = create_string_buffer(n+64)
    p = create_string_buffer(64)

    # expand
    c = expand(m)

    for j in range(n+64): x[j] = randbyte()
    for j in range(n): x[j] = m[j]
    xlen = c_longlong(n)
    libsecded.secded_expand(x,byref(xlen))
    assert xlen.value == len(c)
    for j in range(len(c)): assert ord(x[j]) == c[j]

    # parity
    for j in range(len(c)): c[j] = randbyte()
    q = parity(c)
    assert len(q) == len(c)-n

    for j in range(n+64): x[j] = randbyte()
    for j in range(len(c)): x[j] = c[j]
    xlen = c_longlong(len(c))
    libsecded.secded_parity(p,x,xlen)
    for j in range(len(q)): assert ord(p[j]) == q[j]

    # fill
    for j in range(len(c)): c[j] = randbyte()
    for j in range(len(q)): q[j] = randbyte()
    d = fill(c,q)
    assert len(d) == len(c)

    for j in range(n+64): x[j] = randbyte()
    for j in range(len(c)): x[j] = c[j]
    xlen = c_longlong(len(c))
    for j in range(len(q)): p[j] = q[j]
    libsecded.secded_fill(x,xlen,p)
    for j in range(len(d)): assert ord(x[j]) == d[j]

    # correct
    for j in range(len(c)): c[j] = randbyte()
    for j in range(len(q)): q[j] = randbyte()
    sec,ded,d = correct(c,q)
    assert len(d) == len(c)

    for j in range(n+64): x[j] = randbyte()
    for j in range(len(c)): x[j] = c[j]
    xlen = c_longlong(len(c))
    for j in range(len(q)): p[j] = q[j]
    secded = libsecded.secded_correct(x,xlen,p)
    for j in range(len(d)): assert ord(x[j]) == d[j]
    assert secded == sec+ded*256

    # shrink
    for j in range(len(c)): c[j] = randbyte()
    t = shrink(c)
    assert len(t) == n

    for j in range(n+64): x[j] = randbyte()
    for j in range(len(c)): x[j] = c[j]
    xlen = c_longlong(len(c))
    libsecded.secded_shrink(x,byref(xlen))
    for j in range(len(t)): assert ord(x[j]) == t[j]

    # encode
    c = encode(m)
    for j in range(n+64): x[j] = randbyte()
    for j in range(n): x[j] = m[j]
    xlen = c_longlong(n)
    libsecded.secded_encode(x,byref(xlen))
    assert xlen.value == len(c)
    for j in range(len(c)): assert ord(x[j]) == c[j]

    # decode and clean, 0 errors
    assert decode(c) == (0,0,m)
    assert clean(c) == (0,0,c)

    for j in range(n+64): x[j] = randbyte()
    for j in range(len(c)): x[j] = c[j]
    xlen = c_longlong(len(c))
    secded = libsecded.secded_decode(x,byref(xlen))
    assert xlen.value == n
    assert secded == 0
    for j in range(len(m)): assert ord(x[j]) == m[j]

    for j in range(n+64): x[j] = randbyte()
    for j in range(len(c)): x[j] = c[j]
    xlen = c_longlong(len(c))
    secded = libsecded.secded_clean(x,xlen)
    assert secded == 0
    for j in range(len(c)): assert ord(x[j]) == c[j]

    # decode, invalid lengths
    for invlen in range(len(c)+1,n+65):
      if (invlen-1)&(invlen-2): break

      assert decode(c+[randbyte() for j in range(len(c),invlen)])[2] == m

      for j in range(n+64): x[j] = randbyte()
      for j in range(len(c)): x[j] = c[j]
      xlen = c_longlong(invlen)
      secded = libsecded.secded_decode(x,byref(xlen))
      assert xlen.value == n
      for j in range(len(m)): assert ord(x[j]) == m[j]

    # decode and clean, 1 error
    if len(c) >= 1:
      r = list(c)
      expectsec = randerrpos()
      for bit in range(8):
        if expectsec&(1<<bit):
          r[randrange(len(r))] ^= 1<<bit
      assert decode(r) == (expectsec,0,m)
      assert clean(r) == (expectsec,0,c)

      for j in range(n+64): x[j] = randbyte()
      for j in range(len(r)): x[j] = r[j]
      xlen = c_longlong(len(r))
      secded = libsecded.secded_decode(x,byref(xlen))
      assert xlen.value == n
      for j in range(len(m)): assert ord(x[j]) == m[j]
      assert secded == expectsec

      for j in range(n+64): x[j] = randbyte()
      for j in range(len(r)): x[j] = r[j]
      xlen = c_longlong(len(r))
      secded = libsecded.secded_clean(x,xlen)
      for j in range(len(c)): assert ord(x[j]) == c[j]
      assert secded == expectsec

    # decode and clean, 2 errors
    if len(c) >= 2:
      r = list(c)
      expectded = randerrpos()
      for bit in range(8):
        if expectded&(1<<bit):
          e1 = randrange(len(r))
          e2 = randrange(len(r))
          while e2 == e1: e2 = randrange(len(r))
          r[e1] ^= 1<<bit
          r[e2] ^= 1<<bit
      sec,ded,_ = decode(r)
      assert sec == 0
      assert ded == expectded
      sec,ded,_ = clean(r)
      assert sec == 0
      assert ded == expectded
      
      for j in range(n+64): x[j] = randbyte()
      for j in range(len(r)): x[j] = r[j]
      xlen = c_longlong(len(r))
      secded = libsecded.secded_decode(x,byref(xlen))
      assert secded == 256*expectded

      for j in range(n+64): x[j] = randbyte()
      for j in range(len(r)): x[j] = r[j]
      xlen = c_longlong(len(r))
      secded = libsecded.secded_clean(x,xlen)
      assert secded == 256*expectded

    # decode, mix of 1 and 2 errors
    if len(c) >= 2:
      r = list(c)
      expectsec = randerrpos()
      expectded = randerrpos()
      expectded &= ~expectsec
      for bit in range(8):
        if expectsec&(1<<bit):
          r[randrange(len(r))] ^= 1<<bit
        if expectded&(1<<bit):
          e1 = randrange(len(r))
          e2 = randrange(len(r))
          while e2 == e1: e2 = randrange(len(r))
          r[e1] ^= 1<<bit
          r[e2] ^= 1<<bit
      sec,ded,_ = decode(r)
      assert sec == expectsec
      assert ded == expectded
      sec,ded,_ = clean(r)
      assert sec == expectsec
      assert ded == expectded
      
      for j in range(n+64): x[j] = randbyte()
      for j in range(len(r)): x[j] = r[j]
      xlen = c_longlong(len(r))
      secded = libsecded.secded_decode(x,byref(xlen))
      assert secded == expectsec+256*expectded
      
      for j in range(n+64): x[j] = randbyte()
      for j in range(len(r)): x[j] = r[j]
      xlen = c_longlong(len(r))
      secded = libsecded.secded_clean(x,xlen)
      assert secded == expectsec+256*expectded
