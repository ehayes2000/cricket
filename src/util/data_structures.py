b# map from offset to info about source
import dataclass

class SrcInfo(dataclass):
  name: str
  line: int
  src_file: str


class Interval(dataclass):
  time: int #:)
  is_start: bool


src_map: dict[str, SrcInfo] = {}
intervals: dict[str, Interval] = {}

# 1. populate src map from src file
# 2. initialize intervals with 0's
# 3.         



# set up breakpoints at low/high pc
# resume child process
# hit breakpoint
# grab the Interval that corresponds to the function we are in
# record the current time
# save the value
# re-insert original instruction
# resume child process
#...
# hit breakpoint
# grab Interval ...
# record current time
# save and print



# stack of timestamps
# push timestamp on function entry
# pop timestamp on function exit, subtract current time and print?
# save timedelta in another stack of measurements
