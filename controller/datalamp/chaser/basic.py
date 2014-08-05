
from datalamp.common import SpiralAddressDecorator
from datalamp.chaser import ChaserInput

INPUT_CHAIN = [
  ChaserInput,
  SpiralAddressDecorator
]

pixels = [(0, 0, 0) for i in range(64)]

def input_chain(config):
  chain = None
  for element in INPUT_CHAIN:
    chain = element(chain, config)
  return chain
	
def on_tick(events):
  global pixels
  for event in events:
    pixels = [(0, 0, 0) for i in range(64)]
    pixels[event["address"]] = (255, 255, 255)
