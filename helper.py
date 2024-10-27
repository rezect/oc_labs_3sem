from numpy import random

x = random.randint(1000000, size = (20000000))

for i in x:
  print(i, end = ' ')