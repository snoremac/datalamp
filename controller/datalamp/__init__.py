
from datalamp.emojitracker import sum
from datalamp.emojitracker import avg
from datalamp.chaser import basic
from datalamp.chaser import trail

TILE_LENGTH = 8
PROGRAMS = { "chaser": basic, "chaser-trail": trail, "emoji-sum": sum, "emoji-avg": avg }

def matrix_pixels(config):
  return (TILE_LENGTH ** 2) * config.row_tiles * config.column_tiles
  
def matrix_width(config):
  return config.row_tiles * TILE_LENGTH
  
def matrix_height(config):
  return config.column_tiles * TILE_LENGTH
