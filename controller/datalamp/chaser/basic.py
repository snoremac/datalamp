
import datalamp
from datalamp.common import SpiralAddressDecorator
from datalamp.chaser import ChaserInput

INPUT_CHAIN = [
  ChaserInput,
  #SpiralAddressDecorator
]

pixels = []

def init(config):
  global num_pixels
  num_pixels = (datalamp.TILE_LENGTH ** 2) * config.tiles

def input_chain(config):
  chain = None
  for element in INPUT_CHAIN:
    chain = element(chain, config)
  return chain
	
def on_tick(events):
  global pixels
  for event in events:
    pixels = [(0, 0, 0) for i in range(num_pixels)]
    pixels[event["address"]] = (255, 255, 255)
