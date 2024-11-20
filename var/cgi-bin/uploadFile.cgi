#!/usr/bin/python3

import os
import sys

# Function to create the storage directory if it doesn't exist
def create_storage_directory():
    storage_path = "www/CGIStorage/"
    if not os.path.exists(storage_path):
        try:
            os.makedirs(storage_path)
            print(f"Storage directory created at: {storage_path}")
        except OSError as e:
            print(f"Error creating storage directory: {str(e)}")
            sys.exit(1)
    return storage_path

# Function to write the file to storage
def save_file(storage_path, file_name, file_content):
    file_path = os.path.join(storage_path, file_name)
    try:
        # Open the file in binary write mode and write the file_content
        with open(file_path, 'wb') as file:
            file.write(file_content)
        print(f"File '{file_name}' successfully saved at '{file_path}'")
    except Exception as e:
        print(f"Error writing to file '{file_name}': {str(e)}")
        sys.exit(1)

# Main function
def main():
    if len(sys.argv) != 4:
        print("Usage: script_name.py <file_name> <file_descriptor> <file_size>")
        sys.exit(1)
    
    file_name = sys.argv[1]
    fd = int(sys.argv[2])
    file_size = int(sys.argv[3])
    print(f"File name: {file_name}, File descriptor: {fd}, File size: {file_size}")
    
    data = b""
    bytes_read = 0
    while bytes_read < file_size:
        chunk = os.read(fd, min(4096, file_size - bytes_read))
        if not chunk:
            break
        data += chunk
        bytes_read += len(chunk)
    
    try:
        os.close(fd)
    except OSError as e:
        print(f"Error closing file descriptor: {e}")

    storage_path = create_storage_directory()
    save_file(storage_path, file_name, data)

if __name__ == "__main__":
    main()
