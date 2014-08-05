
from datalamp.emojitracker import EmojiTrackerInput
from datalamp.emojitracker import RankDecorator
from datalamp.emojitracker import SumDecorator

INPUT_CHAIN = [
  EmojiTrackerInput,
  RankDecorator,
  SumDecorator
]

pixels = [(0, 0, 0) for i in range(64)]
lit_pixels = [(0, 0, 0, 0) for i in range(3)]

def input_chain(config):
  chain = None
  for element in INPUT_CHAIN:
    chain = element(chain, config)
  return chain
	
def on_tick(events):
	for event in events:
		pixels[event["rank"]] = (255, 255, 255)
		lit_pixel = lit_pixels.pop(0)
		pixels[lit_pixel[0]] = (lit_pixel[1], lit_pixel[2], lit_pixel[3])
		
		sum = event["sum"]
		if sum < 127:
			green = 255
			red = 255 - ((128 - sum) * 2)
		else:
			green = 255 - ((sum - 128) * 2)
			red = 255		
		lit_pixels.append((event["rank"], red, green, 0))
