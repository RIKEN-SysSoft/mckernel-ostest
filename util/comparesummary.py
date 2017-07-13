#!/usr/bin/python

import sys

if len(sys.argv) != 3:
    print sys.argv[0], "summary1.txt summary2.txt"

f1 = open(sys.argv[1], 'r')
f2 = open(sys.argv[2], 'r')

for line1 in f1:
    line1 = line1.rstrip('\n')
    line2 = f2.readline().rstrip('\n')
    if line1 != line2:
        print '-', line1
        print '+', line2

for line2 in f2:
    line2 = line2.rstrip('\n')
    print '-'
    print '+', line2


