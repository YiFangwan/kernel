import sys
import string

if sys.argv[1:]:
    source = sys.argv[1];
else:
   raise Error, "It's necessarily define first argument"

if sys.argv[2:]:
    dest = sys.argv[2];
else:
   raise Error, "It's necessarily define second argument"

savestdin = sys.stdin
while 1:
    s = savestdin.readline()
    if s:
        s = s.replace( source, dest )
	sys.stdout.write( s )
    else:
        break