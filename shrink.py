def shrink(x):
  x = list(x)
  if len(x) == 0: return []

  bits = 0
  while (1<<bits) < len(x):
    bits += 1

  while bits > 0:
    bits -= 1
    x.pop(1<<bits)
  x.pop(0)
  return x
