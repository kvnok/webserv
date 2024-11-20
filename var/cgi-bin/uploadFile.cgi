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
        with open(file_path, 'wb') as file:
            for byte in file_content.encode('utf-8'):
                print(byte)
                file.write(bytes([byte]))
        print(f"File '{file_name}' successfully saved at '{file_path}'")
    except Exception as e:
        print(f"Error writing to file '{file_name}': {str(e)}")
        sys.exit(1)

# Main function
def main():
    # Check if the correct number of arguments are provided
    if len(sys.argv) != 3:
        print("Usage: script_name.py <file_name> <file_content>")
        sys.exit(1)

    # Argument 1: File name
    file_name = sys.argv[1]
    file_content = sys.argv[2]
    storage_path = create_storage_directory()
    save_file(storage_path, file_name, file_content)


if __name__ == "__main__":
    main()