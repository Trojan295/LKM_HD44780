#!/usr/bin/python

with open('/dev/hd44780', 'w') as f:
	f.write("Example text")
