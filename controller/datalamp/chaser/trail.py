
import datalamp
from datalamp.common import SpiralAddressDecorator
from datalamp.chaser import ChaserInput

INPUT_CHAIN = [
  ChaserInput,
  SpiralAddressDecorator
]

pixels = []

def init(config):
  global pixels, lit_pixels
  lit_pixels = 0
  pixels = [(0, 0, 0) for i in range(datalamp.matrix_pixels(config))]

def input_chain(config):
  chain = None
  for element in INPUT_CHAIN:
    chain = element(chain, config)
  return chain
	
def on_tick(events):
  global pixels, lit_pixels
  
  for event in events:
    if lit_pixels == len(pixels):
      pixels = [(0, 0, 0) for i in range(len(pixels))]
      lit_pixels = 0
    pixels[event["address"]] = (100, 100, 100)
    lit_pixels += 1
