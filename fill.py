def fill(x,p):
  x = list(x)
  if len(x) == 0: return []

  overall = 0
  bit = 0
  while (1<<bit) < len(x):
    overall ^= p[bit]
    x[1<<bit] = p[bit]
    bit += 1

  overall ^= p[bit]
  x[0] = overall
  return x
