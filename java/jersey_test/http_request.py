#! /usr/bin/env python

import sys
import telnetlib

num_argv = len(sys.argv)

if num_argv == 2:
    host = sys.argv[1]
    port = 80
elif num_argv == 3:
    host = sys.argv[1]
    port = sys.argv[2]
else:
    print("Usage: host [port]");
    sys.exit(1)

telnet = telnetlib.Telnet(host, port)

while True:
    line = sys.stdin.readline()
    if not line:
        break
    telnet.write(line)

for line in telnet.read_all():
    sys.stdout.write(line)

print("");
