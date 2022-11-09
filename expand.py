def expand(m):
  m = list(m)
  if len(m) == 0: return []
  c = [0]+m
  bits = 0
  while (1<<bits) < len(c):
    c.insert(1<<bits,0)
    bits += 1
  return c
