#!/usr/bin/env python

import pprint

pp = pprint.PrettyPrinter(indent = 4)

spacing = 0.2
lines = []
for row in reversed(range(8)):
  for column in reversed(range(-4, 4)):
    lines.append('  {"point": [%.2f, %.2f, %.2f]}' % (column * spacing, 0, row * spacing))
print '[\n' + ',\n'.join(lines) + '\n]'

