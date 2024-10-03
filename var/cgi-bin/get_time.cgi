#!/usr/bin/env python3
import datetime

# Output the necessary headers
print("Content-Type: text/html")    # HTML is following
print()                             # Blank line to end headers

# Output the current date and time in HTML format
print(f"<html><body><h1>Current time: {str(datetime.datetime.now())}</h1></body></html>")
