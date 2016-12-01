#!/usr/bin/env python

import serial
import argparse
import sys
import time

parser = argparse.ArgumentParser(description='Send an mp3 file over serial UART and receive the decoded WAV samples over ethernet.')
parser.add_argument('mp3_file', type=str, help='mp3 file to send')
parser.add_argument('--serial', type=str, default='/dev/ttyUSB0', help='serial port')
parser.add_argument('--baud', type=int, default=1000000, help='baud')
parser.add_argument('--address', type=str, default='localhost', help='network address')
parser.add_argument('--port', type=int, default=1234, help='network port')
parser.add_argument('--blocksize', type=int, default=128, help='transmission block size')

args = parser.parse_args()

print('Starting socket on {}:{}'.format(args.address, args.port))

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((args.address, args.port))

print('')
print('Sending "{}" over serial'.format(args.mp3_file))
print('  device:     {}'.format(args.serial))
print('  baud:       {}'.format(args.baud))
print('  block size: {}'.format(args.blocksize))

try:
    with open(args.mp3_file, 'r') as f, serial.Serial(args.serial, args.baud, timeout=1) as ser:
        while True:
            s = f.read(blocksize)
            if s == "":
                break
            ser.write(s)
except IOError as e:
    print('')
    print(e)
    sys.exit(0)

print('')
print('Waiting a bit before closing socket')
time.sleep(5)

sock.close()
