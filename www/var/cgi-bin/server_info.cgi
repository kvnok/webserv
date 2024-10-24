#!/usr/bin/env python3
import os
print("Content-Type: text/html")    
print()                             
print("<html><body><h1>Server Information</h1><pre>")
for key, value in sorted(os.environ.items()):
	if key.startswith("SERVER"):
		print(f"{key}={value}")
print("</pre></body></html>")