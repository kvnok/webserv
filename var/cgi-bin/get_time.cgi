#!/usr/bin/env python3
import datetime

# Output the current date and time in HTML format
print(f"Content-Type: text/html")    
print(f"<html><body><h1><br><br><br><br>Current time: {str(datetime.datetime.now())}</h1></body></html>")
