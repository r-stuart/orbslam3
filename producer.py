import argparse
import os
import sys

HEADER_SIZE = 8
ULLI_MAX = (2 << 63) - 1

def send(data):
    sys.stdout.buffer.write(data)
    sys.stdout.flush()

def to_bytes(number):
    return number.to_bytes(HEADER_SIZE, byteorder='big')

def main(root):
    ims = list(sorted([f for f in os.listdir(root) if f.endswith('.png')]))
    for im in ims:
        with open(os.path.join(root, im), 'rb') as f:
            ts = to_bytes(int(im.split('.')[0]))
            loaded_im = f.read()
            length = to_bytes(len(loaded_im))
        send(ts)
        send(length)
        send(loaded_im)
    send(to_bytes(ULLI_MAX))

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("path")
    args = parser.parse_args()
    main(args.path)
