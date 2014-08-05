

class SpiralAddressDecorator(object):
  
  def __init__(self, chain, config):
    self.chain = chain
    self.spiral_addresses = self.to_spiral_addresses(self.horizontal_matrix())
    
  def __iter__(self):
    return self
    
  def __next__(self):
    return self.next()

  def next(self):
    event = self.chain.next()
    if "address" in event:
      event["address"] = self.spiral_addresses[event["address"]]
    return event

  def horizontal_matrix(self):
    matrix = []
    for row in range(8):
      matrix.append([])
      for column in range(8):
        matrix[row].append((row * 8) + column)
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

