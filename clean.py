from parity import parity
from correct import correct

def clean(x):
  return correct(x,parity(x))
