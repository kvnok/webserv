#!/usr/bin/python3

import os
import sys

def create_storage_directory():
    storage_path = "www/CGIStorage/"
    if not os.path.exists(storage_path):
        try:
            os.makedirs(storage_path)
            print(f"Storage directory created at: {storage_path}")
        except OSError as e:
            print(f"Error creating storage directory: {str(e)}")
            sys.exit(501)
    return storage_path

def save_file(storage_path, file_name, file_content):
    file_path = os.path.join(storage_path, file_name)
    try:
        with open(file_path, 'wb') as file:
            file.write(file_content)
        print(f"File '{file_name}' successfully saved at '{file_path}'")
    except Exception as e:
        print(f"Error writing to file '{file_name}': {str(e)}")
        sys.exit(501)

def main():
    if len(sys.argv) != 3:
        print("Not all the required arguments were provided")
        sys.exit(501)
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
    sys.exit(201)

if __name__ == "__main__":
    main()