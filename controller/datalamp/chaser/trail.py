
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
  center_address = 35
  
  for event in events:
    if event["address"] == center_address:
      pixels = [(0, 0, 0) for i in range(64)]  
    pixels[event["address"]] = (255, 255, 255)
