#!/usr/bin/env python3
import os
print("Content-Type: text/html")    
print()                             
print("<html><body><h1>Path Information</h1><br><h3>")
print(os.getenv("PATH_INFO"))
print("</h3></body></html>", end="")