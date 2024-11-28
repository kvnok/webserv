#!/usr/bin/env python3
import os
print(f"Content-Type: text/html")
print()
print("<html><body><h1>Environment Variables</h1><ol>")
for key, value in sorted(os.environ.items()):
	print(f"<li>{key}={value}</li>")
print("<ol></body></html>", end="")