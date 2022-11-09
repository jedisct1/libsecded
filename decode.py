from parity import parity
from correct import correct
from shrink import shrink

def decode(x):
  sec,ded,x = correct(x,parity(x))
  return sec,ded,shrink(x)
