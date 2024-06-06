#!/usr/bin/env python3
import os
print("Content-Type: text/html")    # HTML is following
print()                             # blank line, end of headers
print("<html><body><h1>Environment Variables</h1><pre>")
for key, value in sorted(os.environ.items()):
	print(f"{key}={value}")
print("</pre></body></html>")