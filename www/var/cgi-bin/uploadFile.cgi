#!/usr/bin/python3

import os
import sys

# Function to create the storage directory if it doesn't exist
def create_storage_directory():
    storage_path = "www/storage/"
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
        with open(file_path, 'w') as file:
            file.write(file_content)
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

    # Argument 2: File content
    file_content = sys.argv[2]

    # Create the storage directory
    storage_path = create_storage_directory()

    # Save the file in the storage directory
    save_file(storage_path, file_name, file_content)


if __name__ == "__main__":
    main()