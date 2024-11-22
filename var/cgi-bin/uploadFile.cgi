#!/usr/bin/python3

import os
import sys

def create_storage_directory():
    storage_path = "www/CGIStorage/"
    if not os.path.exists(storage_path):
        try:
            os.makedirs(storage_path)
        except OSError as e:
            print(f"500")
            sys.exit(-1)
    return storage_path

def save_file(storage_path, file_name, file_content):
    file_path = os.path.join(storage_path, file_name)
    try:
        with open(file_path, 'wb') as file:
            file.write(file_content)
        print(f"201")
    except Exception as e:
        print(f"500")
        sys.exit(-1)

def main():
    if len(sys.argv) != 3:
        print("500")
        sys.exit(-1)
    file_name = sys.argv[1]
    file_size = int(sys.argv[2])
    
    data = b""
    bytes_read = 0
    while True:
        chunk = os.read(0, min(4096, file_size - bytes_read))
        if not chunk:
            break
        data += chunk
        bytes_read += len(chunk)
        if bytes_read >= file_size:
            break
    storage_path = create_storage_directory()
    save_file(storage_path, file_name, data)

if __name__ == "__main__":
    main()