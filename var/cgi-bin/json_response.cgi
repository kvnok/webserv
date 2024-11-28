#!/usr/bin/env python3
import json
data = {"name": "John", "age": 30, "city": "New York"}
print("Content-Type: application/json")    
print()                             
print(json.dumps(data), end="")