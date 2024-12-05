#!/usr/bin/python3
import os
import sys

def create_storage_directory():
    storage_path = "var/CGIStorage/"
    if not os.path.exists(storage_path):
        try:
            os.makedirs(storage_path)
        except OSError as e:
            print(f"500", end="")
            sys.exit(-1)
    return storage_path

def save_file(storage_path, file_name, file_content):
    file_path = os.path.join(storage_path, file_name)
    if os.path.exists(file_path):
        print(f"409", end="")
        sys.exit(-1)
    try:
        with open(file_path, 'wb') as file:
            file.write(file_content)
        if file_name.endswith(".ico") or file_name.endswith(".jpg") or file_name.endswith(".jpeg"):
            relative_path = "../" + file_path.split("var/")[1]
            content_html = f"<html><body>"
            content_html += f"<h1>{file_name} uploaded successfully!</h1>"
            content_html += f"<p>File path: {relative_path}</p>"
            content_html += f"<img src='{relative_path}' style='width: 10%; height: 10%'>"
            content_html += f"</body></html>"
            print("Content-Type: text/html")
            print(f"Content-Length: {len(content_html)}")
            print()
            print(f"{content_html}", end="")
        else:
            print("201", end="")
    except Exception as e:
        print(f"500", end="")
        sys.exit(-1)

def main():
    if len(sys.argv) != 1:
        print("500", end="")
        sys.exit(-1)
    method = os.getenv('METHOD')
    file_name = os.getenv('FILE_NAME')
    file_size = int(os.getenv('BODY_SIZE'))
    if file_size:
        print("500", end="")
        sys.exit(-1)
    if not file_name:
        print("400", end="")
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