
import datalamp
from datalamp.common import SpiralAddressDecorator
from datalamp.common import SumDecorator
from datalamp.emojitracker import EmojiTrackerInput
from datalamp.emojitracker import RankAddressDecorator

INPUT_CHAIN = [
  EmojiTrackerInput,
  RankAddressDecorator,
  SpiralAddressDecorator,
  SumDecorator
]

pixels = []

def init(config):
  global pixels, lit_pixels
  pixels = [(0, 0, 0) for i in range(datalamp.matrix_pixels(config))]
  lit_pixels = [(0, 0, 0, 0) for i in range(3)]

def input_chain(config):
  chain = None
  for element in INPUT_CHAIN:
    chain = element(chain, config)
  return chain
	
def on_tick(events):
  for event in events:
    pixels[event["address"]] = (0,0, 0)
    lit_pixel = lit_pixels.pop(0)
    pixels[lit_pixel[0]] = (lit_pixel[1], lit_pixel[2], lit_pixel[3])

    sum = event["sum"]
    if sum > 255:
      sum = 255

    if sum < 127:
    	green = 255
    	red = 255 - ((128 - sum) * 2)
    else:
    	green = 255 - ((sum - 128) * 2)
    	red = 255

    green = int(round((sum / 255.0) * green))
    red = int(round((sum / 255.0) * red))
    	
    lit_pixels.append((event["address"], red, green, 0))
