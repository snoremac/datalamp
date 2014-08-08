
import math
import time

import datalamp

class SpiralAddressDecorator(object):
  
  def __init__(self, chain, config):
    self.chain = chain
    
    self.spiral_addresses = self.to_spiral_addresses(self.tiled_matrix(
        config.row_tiles,
        config.column_tiles,
        datalamp.TILE_LENGTH))
    
  def __iter__(self):
    return self
    
  def __next__(self):
    return self.next()

  def next(self):
    event = self.chain.next()
    if "address" in event:
      event["address"] = self.spiral_addresses[event["address"]]
    return event

  def horizontal_matrix(self, rows, columns):
    matrix = []
    for row in range(rows):
      matrix.append([])
      for column in range(columns):
        matrix[row].append((row * columns) + column)
    return matrix
    
  def tiled_matrix(self, row_tiles, column_tiles, tile_length):
    matrix = []
    i = 0

    for row_tile in range(row_tiles):
      for column_tile in range(column_tiles):
        for row in range(0, tile_length):
          if column_tile == 0:
            matrix.append([])
          for column in range(0, tile_length):
            matrix[(row_tile * tile_length) + row].append(i)
            i += 1
    
    return matrix

  def to_spiral_addresses(self, matrix):
    spiral = []

    start_row = 0
    end_row = len(matrix)
    start_column = 0
    end_column = len(matrix[0])
    
    while (start_row < end_row and start_column < end_column):
      
      for i in range(start_column, end_column):
        spiral.append(matrix[start_row][i])
      start_row += 1
    
      for i in range(start_row, end_row):
        spiral.append(matrix[i][end_column - 1])
      end_column -= 1

      if (start_row < end_row):
        for i in range(end_column - 1, start_column -1, -1):
          spiral.append(matrix[end_row - 1][i])
        end_row-= 1

      if (start_column < end_column):
        for i in range(end_row - 1, start_row - 1, -1):
          spiral.append(matrix[i][start_column])
        start_column += 1
        
    spiral.reverse()
    return spiral


class SumDecorator(object):

  def __init__(self, chain, config):
    self.chain = chain
    self.sums = {}
    
  def __iter__(self):
    return self
  
  def __next__(self):
    return self.next()

  def next(self):
    event = self.chain.next()
    if event["id"] not in self.sums:
      self.sums[event["id"]] = 0
    self.sums[event["id"]] += 1
    event["sum"] = self.sums[event["id"]]
    return event

class MovingAverageDecorator(object):
  
  def __init__(self, chain, config):
    self.chain = chain
    self.window = float(config.average_window)
    self.window_events = {}
    
  def __iter__(self):
    return self
  
  def __next__(self):
    return self.next()

  def next(self):
    event = self.chain.next()
    if event["id"] not in self.window_events:
      self.window_events[event["id"]] = []

    now = time.time()
    window_boundary = now - (self.window / 1000.0)
    self.window_events[event["id"]] = [x for x in self.window_events[event["id"]] if x > window_boundary]
    self.window_events[event["id"]].append(now)
    
    event["avg"] = len(self.window_events[event["id"]])
    return event
