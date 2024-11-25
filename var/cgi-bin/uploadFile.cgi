#!/usr/bin/python3
import os
import sys

def create_storage_directory():
    storage_path = "var/CGIStorage/"
    if not os.path.exists(storage_path):
        try:
            os.makedirs(storage_path)
        except OSError as e:
            print(f"500")
            sys.exit(-1)
    return storage_path

def save_file(storage_path, file_name, file_content):
    file_path = os.path.join(storage_path, file_name)
    if os.path.exists(file_path):
        print(f"409")
        sys.exit(-1)
    try:
        with open(file_path, 'wb') as file:
            file.write(file_content)
        if file_name.endswith(".ico") or file_name.endswith(".jpg") or file_name.endswith(".jpeg"):
            relative_path = "../" + file_path.split("var/")[1]
            print("Content-Type: text/html")
            print(f"Content-Length: {len(file_content)}")
            print()
            print(f"<html><body>")
            print(f"<h1>{file_name} uploaded successfully!</h1>")
            print(f"<p>File path: {relative_path}</p>")
            print(f"<img src='{relative_path}' style='width: 10%; height: 10%'>")
            print("</body></html>")
        else:
            print(f"201")
    except Exception as e:
        print(f"500")
        sys.exit(-1)

def main():
    if len(sys.argv) != 1:
        print("500")
        sys.exit(-1)
    file_name = os.getenv('FILE_NAME')
    file_size = int(os.getenv('BODY_SIZE'))
    if not file_name or not file_size:
        print("400")
        sys.exit(-1)
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