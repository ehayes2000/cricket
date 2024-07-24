import io
import sys
import subprocess
import csv
import numpy as np
import matplotlib.pyplot as plt


CRICKET_BIN = "bin/cricket"
OUT_CSV = "outputs/calls.csv"
args = []

def append_prio_queue(q: list[float], time: float) -> list[float]:
  q.append(time)
  q.sort()
  return q

def flame_graph_calls(calls):
    calls.sort(key=lambda c: c[1])
    fig, ax = plt.subplots(figsize=(12,6))
    prio_q = []
    for name, start, dur in calls:
        end = start + dur
        while len(prio_q) > 0 and prio_q[0] <= start:
            prio_q.pop(0)
        y_idx = len(prio_q)
        prio_q = append_prio_queue(prio_q, end)
        ax.barh(y_idx, dur, left=start, height=1, edgecolor='white', linewidth=0.5)
        if dur > 2:
          ax.text(start + dur/2, y_idx, name, ha='center', va='center', 
                fontsize=6, color='black')
    ax.set_ylim(0, max(len(prio_q), 1))
    ax.set_yticks([])
    ax.set_xlabel('Time')
    ax.set_title('Flame Graph of Function Calls')
    plt.tight_layout()
    plt.savefig("flameo.png", dpi=300, bbox_inches='tight')
    
if __name__ == "__main__":
  if not len(sys.argv) == 2:
    raise Exception("usage: garph_calls <executable>")
  executable = sys.argv[1]

  with open(OUT_CSV, "w") as out:
    subprocess.run(
      [CRICKET_BIN, *args, executable],
      stdout=out
    )
  name_cpu_time = []
  with open(OUT_CSV, "r") as out: 
    callreader = csv.reader(out, delimiter=",")
    next(callreader, None) # skip header
    for row in callreader:
      c_start_s, c_start_us = row[1].split(" ")
      c_dur_s, c_dur_us = row[2].split(" ")
      c_start_ms = int(c_start_s) * 1000 + int(c_start_us) / 1000
      c_dur_ms = int(c_dur_s) * 1000 + int(c_dur_us) / 1000
      name_cpu_time.append(
        (row[0], c_start_ms, c_dur_ms)
      )
  flame_graph_calls(name_cpu_time)  

     
