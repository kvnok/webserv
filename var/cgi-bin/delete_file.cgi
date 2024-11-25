#!/usr/bin/python3
import os
import sys

def check_file_is_in_CGI_folder(file_name):
    storage = "var/CGIStorage/"
    if not os.path.exists(storage):
        return False
    if not file_name in os.listdir(storage):
        return False
    return True

def delete_specific_file(file_name):
    if not check_file_is_in_CGI_folder(file_name):
        print("409")
        sys.exit(-1)
    file_name = os.path.join("var/CGIStorage/", file_name)
    try:
        os.remove(file_name)
        print("Content-Type: text/html")
        print()
        print(f"File '{file_name}' has been deleted.")
        sys.exit(0)
    except FileNotFoundError:
        print("Content-Type: text/html")
        print()
        print(f"File '{file_name}' not found.")
        sys.exit(-1)
    except PermissionError:
        print("Content-Type: text/html")
        print()
        print(f"Permission denied: cannot delete '{file_name}'.")
        sys.exit(-1)
    except Exception as e:
        print("Content-Type: text/html")
        print()
        print(f"An error occurred: {e}")
        sys.exit(-1)

def main():
    file_name = os.getenv('FILE_NAME')
    if not file_name:
        print("401")
        sys.exit(-1)
    print(f"210")
    # delete_specific_file(file_name)

if __name__ == "__main__":
    main()
