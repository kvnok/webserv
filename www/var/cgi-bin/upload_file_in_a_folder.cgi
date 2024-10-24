#!/usr/bin/env python3

import os
import shutil
import sys

def save_file(file_path, destination_folder):
    """
    Accepts a file and saves it in a specific folder.

    :param file_path: Path to the file to be saved
    :param destination_folder: Folder where the file should be saved
    """
    # print(f"File Name: {file_name}")
    file_path = "." + file_path
    if not os.path.isfile(file_path):
        print(f"Error: {file_path} is not a valid file.")
        return
    
    if not os.path.isdir(destination_folder):
        os.makedirs(destination_folder)
    
    # Extract file name from the file path
    file_name = os.path.basename(file_path)
    
    # Construct the full path for the destination
    destination_path = os.path.join(destination_folder, file_name)
    # Print the file name and file path
    # Copy the file to the destination folder
    try:
        shutil.copy(file_path, destination_path)
        print(f"File {file_name} has been saved to {destination_folder}.")
    except Exception as e:
        print(f"Error: Could not save the file. {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python save_file_script.py <file_path> <destination_folder>")
        sys.exit(1)

    file_path = sys.argv[1]
    destination_folder = sys.argv[2]

    save_file(file_path, destination_folder)