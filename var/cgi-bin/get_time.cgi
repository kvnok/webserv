#!/usr/bin/env python3
import datetime
print("Content-Type: text/html")    # HTML is following
print()                             # blank line, end of headers
print("<html><body><h1>Current time: " + str(datetime.datetime.now()) + "</h1></body></html>")