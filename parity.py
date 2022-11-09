def parity(x):
  x = list(x)
  if len(x) == 0: return []

  bits = 0
  while (1<<bits) < len(x):
    bits += 1

  overall = 0
  for j in range(len(x)):
    overall ^= x[j]

  result = [0]*bits
  for bit in range(bits):
    for j in range(len(x)):
      if j&(1<<bit):
        result[bit] ^= x[j]

  return result+[overall]
