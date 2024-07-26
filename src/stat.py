import io
import sys
import subprocess
import csv
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict

CRICKET_BIN = "bin/cricket"
OUT_CSV = "outputs/stat_calls.csv"
args = []

if __name__ == "__main__":
  if not len(sys.argv) == 2:
    raise Exception("usage: garph_calls <executable>")
  executable = sys.argv[1]
  with open(OUT_CSV, "w") as out:
    subprocess.run(
      [CRICKET_BIN, *args, executable],
      stdout=out
    )
  calls = defaultdict(lambda: 0)
  with open(OUT_CSV, "r") as out:
    reader = csv.reader(out, delimiter=",")
    next(reader, None)
    for row in reader:
      calls[row[0]] += 1
  
  for k, v in calls.items():
    print(f"{k}: {v}")
      
