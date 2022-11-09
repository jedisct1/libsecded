from expand import expand
from parity import parity
from fill import fill

def encode(x):
  x = expand(x)
  return fill(x,parity(x))
