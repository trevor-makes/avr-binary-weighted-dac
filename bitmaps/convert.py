from PIL import Image
import sys

REC_SIZE = 32

if len(sys.argv) != 2:
  print(f"Usage: python3 {sys.argv[0]} [filename]")
  quit()

# Open filename passed as command line argument
im = Image.open(sys.argv[1])
(w, h) = im.size

# Expect 1-bit image
for (_, c) in im.getcolors():
  assert(c in (0, 1))

# Print bitmap as C PROGMEM array
bytes = []
print('const uint8_t ROM[BITMAP_BYTES] PROGMEM = {')
for y in range(0, h):
  print(' ', end='')
  for byte_idx in range(0, w // 8):
    byte = 0
    for bit_idx in range(0, 8):
      x = byte_idx * 8 + bit_idx
      byte = byte << 1 | im.getpixel((x, y))
    bytes.append(byte)
    print(f' 0x{byte:02X},', end='')
  print()
print('};')

# Print bitmap as IHX string for CLI 'import' command
address = 0
while address < len(bytes):
  rec = bytes[address:address+REC_SIZE]
  size = len(rec)

  print(f':{size:02X}{address:04X}00', end='')
  checksum = size + (address >> 8) + (address & 0xFF)
  address += REC_SIZE

  for byte in rec:
    print(f'{byte:02X}', end='')
    checksum += byte

  checksum = -checksum & 0xFF
  print(f'{checksum:02X}')
print(':00000001FF')
