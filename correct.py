def correct(x,p):
  x = list(x)
  if len(x) == 0: return 0,0,[]
  bits = 0
  while (1<<bits) < len(x):
    bits += 1

  sec = 0
  ded = 0
  overall = p[bits]

  for j in range(1<<bits):
    flip = -1
    for bit in range(bits):
      if j&(1<<bit):
        flip &= p[bit]
      else:
        flip &= ~p[bit]
    if j < len(x):
      if j != 0:
        ded |= flip & ~overall
      sec |= flip & overall
      x[j] ^= flip & overall
    else:
      ded |= flip

  # alternative computation of sec,ded:

  anyp = 0
  borrow = 0
  for bit in range(bits):
    nmask = -(1&(len(x)>>bit))
    pmask = p[bit]
    anyp |= pmask
    borrow = (pmask&nmask&borrow)|((nmask|borrow)&~pmask)

  nmask = -(1&(len(x)>>bits))
  borrow = nmask|borrow

  assert sec == borrow&overall
  assert ded == (anyp&~overall)|(overall&~borrow)

  return sec,ded,x
