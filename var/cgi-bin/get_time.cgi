#!/usr/bin/env python3
import datetime

# Output the current date and time in HTML format
print("Content-Type: text/html\r\n")    
print(f"<html><body><h1><br><br><br><br>Current time: {str(datetime.datetime.now())}</h1></body></html>")
