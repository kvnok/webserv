#!/usr/bin/env python3
import datetime

current_time = datetime.datetime.now()
formatted_time = current_time.strftime("%B %d, %Y, %I:%M %p")
content_html = f"<html><body><h1><br><br><br><br>Current time: {formatted_time}</h1></body></html>"
print(f"Content-Type: text/html")
print(f"Content-Length: {len(content_html)}")
print()
print(f"{content_html}")
