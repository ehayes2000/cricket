import sys

INSTN_SIZE = 4 # bytes

def read_elf_bytes(elf_file_path, offset, num_bytes):
    try:
        with open(elf_file_path, 'rb') as elf_file:
            elf_file.seek(offset)
            data = elf_file.read(num_bytes)
        return data
    except IOError as e:
        print(f"Error reading file: {e}")
        return None

def main():
    if len(sys.argv) != 4:
        print("Usage: python script.py <elf_file_path> <offset> <num_bytes>")
        sys.exit(1)

    elf_file_path = sys.argv[1]
    offset = int(sys.argv[2], 0)  # Accept decimal, octal, or hex
    num_bytes = int(sys.argv[3])

    data = read_elf_bytes(elf_file_path, offset, num_bytes)
    
    print(f"Bytes at offset 0x{offset:x}:")
    for i in range(0, len(data), INSTN_SIZE):
      print("".join(f"{byte:02x} " for byte in data[i: i + INSTN_SIZE]))
    

if __name__ == "__main__":
    main()