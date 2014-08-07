
import datalamp
from datalamp.common import SpiralAddressDecorator
from datalamp.common import MovingAverageDecorator
from datalamp.emojitracker import EmojiTrackerInput
from datalamp.emojitracker import RankAddressDecorator

INPUT_CHAIN = [
  EmojiTrackerInput,
  RankAddressDecorator,
  SpiralAddressDecorator,
  MovingAverageDecorator
]

pixels = []

def init(config):
  global sensitivity
  coefficient = float(config.average_window) / 40
  sensitivity = coefficient / float(config.average_sensitivity)
  
  global pixels
  pixels = [(0, 0, 0) for i in range((datalamp.TILE_LENGTH ** 2) * config.tiles)]  

def input_chain(config):
  chain = None
  for element in INPUT_CHAIN:
    chain = element(chain, config)
  return chain

def on_tick(events):
  for event in events:
    avg = int(round((event["avg"] / sensitivity) * 255))

    if avg < 127:
    	green = 255
    	red = 255 - ((128 - avg) * 2)
    else:
    	green = 255 - ((avg - 128) * 2)
    	red = 255

    green = int(round((avg / 255.0) * green))
    red = int(round((avg / 255.0) * red))
    	
    pixels[event["address"]] = (red, green, 0)
