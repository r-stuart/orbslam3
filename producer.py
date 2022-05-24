import os
import sys
import time

HEADER_SIZE = 8

def send(data):
    sys.stdout.buffer.write(data)
    sys.stdout.flush()

root = '/mnt/slow/MH_01_easy/mav0/cam0/data/'
ims = list(sorted([f for f in os.listdir(root) if f.endswith('.png')]))
print(len(ims), file=sys.stderr)
for im in ims:
    with open(os.path.join(root, im), 'rb') as f:
        ts = (int(im.split('.')[0])).to_bytes(HEADER_SIZE, byteorder='big')
        loaded_im = f.read()
        length = (len(loaded_im)).to_bytes(HEADER_SIZE, byteorder='big')
    send(ts)
    send(length)
    send(loaded_im)
