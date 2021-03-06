
import time
from collections import OrderedDict

import datalamp

class ChaserInput(object):
  
  def __init__(self, chain, config):
    self.event_delay = 1.0 / int(config.throttle) if int(config.throttle) > 0 else 0
    self.max_index = datalamp.matrix_pixels(config)
    self.last_index = 0
    
  def __iter__(self):
    return self
  
  def __next__(self):
    return self.next()

  def next(self):
    end_delay = time.time() + self.event_delay
    while (time.time() < end_delay):
			pass
      
    chaser_dict = OrderedDict({ "address": self.last_index })
    self.last_index += 1
    if (self.last_index == self.max_index):
      self.last_index = 0
    
    return chaser_dict