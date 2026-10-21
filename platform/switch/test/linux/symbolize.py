#!/usr/bin/env python3
"""Resolves '(smw+0xOFFSET)' frames in a sanitizer log with addr2line. Usage: symbolize.py BINARY < log"""
import re, subprocess, sys

binary = sys.argv[1]
text = sys.stdin.read()
offsets = sorted(set(re.findall(r'\((?:[^()]*/)?smw\+(0x[0-9a-f]+)\)', text)))
resolved = {}
if offsets:
    out = subprocess.run(['addr2line', '-f', '-C', '-e', binary] + offsets, capture_output=True, text=True).stdout.splitlines()
    for i, off in enumerate(offsets):
        func, loc = out[2 * i], out[2 * i + 1]
        resolved[off] = f'{func} {loc.replace("/src/", "")}'
print(re.sub(r'\((?:[^()]*/)?smw\+(0x[0-9a-f]+)\)(?: \(BuildId: [0-9a-f]+\))?',
             lambda m: resolved.get(m.group(1), m.group(0)), text), end='')
