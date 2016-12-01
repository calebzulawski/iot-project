#!/usr/bin/env python

import argparse
import sys
import time
import socket

parser = argparse.ArgumentParser(description='Send an mp3 file over a socket')
parser.add_argument('mp3_file', type=str, help='mp3 file to send')
parser.add_argument('--address', type=str, default='localhost', help='network address')
parser.add_argument('--port', type=int, default=1234, help='network port')
parser.add_argument('--blocksize', type=int, default=128, help='transmission block size')

args = parser.parse_args()

print('Starting socket on {}:{}'.format(args.address, args.port))

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((args.address, args.port))

try:
    with open(args.mp3_file, 'r') as f:
        while True:
            data = f.read(blocksize)
            if data == '':
                break
            sock.sendall(data)
except IOError as e:
    print('')
    print(e)
    sys.exit(0)

sock.close()
