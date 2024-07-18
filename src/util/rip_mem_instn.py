import sys
import struct

INSTN_SIZE = 4 # bytes

def read_process_memory(pid, address, size):
    with open(f"/proc/{pid}/mem", "rb") as mem:
        mem.seek(address)
        return mem.read(size)


def main(pid, address, nbytes):
  data = read_process_memory(pid, address, nbytes)
  for i in range(0, len(data), INSTN_SIZE):
    print(''.join(f"{byte:02x} " for byte in data[i: i + INSTN_SIZE])) 
  # print(hex_data)
  # hex_data = ' '.join(f'{byte:02x}' for byte in data)

if __name__ == "__main__":
  if len(sys.argv) != 4:
    raise Exception("usage: <pid> <0xaddress> <nbytes>")
  # Usage
  pid = int(sys.argv[1])
  address = int(sys.argv[2], 16)
  nbytes = int(sys.argv[3])
  print(f"pid [{pid}], addr [{address:02x}], nbytes [{nbytes}]")
  main(pid, address, nbytes)

