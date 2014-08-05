
import gzip
import json
import re
import time
import urllib

class EmojiTrackerInput(object):
  
  def __init__(self, chain, config):
    if config.feed_url.startswith("http://"):
      self.stream = urllib.urlopen(config.feed_url)
    elif config.feed_url.endswith(".gz"):
      self.stream = gzip.open(config.feed_url)
    else:
      self.stream = open(config.feed_url)

    self.event_delay = 1.0 / int(config.throttle) if int(config.throttle) > 0 else 0
    
  def __iter__(self):
    return self
  
  def __next__(self):
    return self.next()

  def next(self):
    end_delay = time.clock() + self.event_delay
    while (time.clock() < end_delay):
			pass
      
    emoji_dict = None
    line = None
    
    while (emoji_dict == None and line != ''):
      line = self.stream.readline()
      match = re.search("data:(.+)", line)
      if (match):
    	 	emoji_dict = dict(emoji_id = match.group(1))
        
    if line == '':
      raise StopIteration()
    else:
      return emoji_dict
      
      
class RankAddressDecorator(object):
  
  def __init__(self, chain, config):
    self.chain = chain
    
    if config.rankings_url.startswith("http://"):
      stream = urllib.urlopen(config.rankings_url)
    elif config.rankings_url.endswith(".gz"):
      stream = gzip.open(config.rankings_url)
    else:
      stream = open(config.rankings_url)
    
    rankings = json.loads(stream.read())
    self.emoji_ranks = { data["id"]: ranking for ranking, data in enumerate(rankings) if int(config.max_emojis) > ranking >= 0 }
    
  def __iter__(self):
    return self
    
  def __next__(self):
    return self.next()

  def next(self):
    emoji_dict = dict(emoji_id = None)
    while emoji_dict["emoji_id"] not in self.emoji_ranks:
      emoji_dict = self.chain.next()
    emoji_dict["address"] = self.emoji_ranks[emoji_dict["emoji_id"]]
    return emoji_dict


class SumDecorator(object):

  def __init__(self, chain, config):
    self.chain = chain
    self.emoji_sums = {}
    
  def __iter__(self):
    return self
  
  def __next__(self):
    return self.next()

  def next(self):
    emoji_dict = self.chain.next()
    if emoji_dict["emoji_id"] not in self.emoji_sums:
      self.emoji_sums[emoji_dict["emoji_id"]] = 0
    self.emoji_sums[emoji_dict["emoji_id"]] += 1
    emoji_dict["sum"] = self.emoji_sums[emoji_dict["emoji_id"]]
    return emoji_dict

