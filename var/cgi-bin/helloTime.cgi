#!/usr/bin/env python3

import os
import sys
from datetime import datetime


def generate_directory_listing_html(directory_path):
    # Get the list of files and directories
    items = os.listdir(directory_path)
    items.sort()

    # Start the HTML document
    html_content = """<!DOCTYPE html>
<html>
<head>
    <title>Index of {0}</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
        }}
        table {{
            width: 100%;
            border-collapse: collapse;
        }}
        th, td {{
            padding: 8px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }}
        th {{
            background-color: #f2f2f2;
        }}
        a {{
            text-decoration: none;
            color: #007bff;
        }}
        a:hover {{
            text-decoration: underline;
        }}  
    </style>
</head>
<body>
    <h1>Index of {0}</h1>
    <table>
        <tr>
            <th>Name</th>
            <th>Last Modified</th>
            <th>Size</th>
        </tr>
""".format(directory_path)  # this will replace all the "{0}" with the directory_paths

    # Add a link to the parent directory
    parent_directory = os.path.dirname(directory_path)
    parent_directory = os.path.dirname(parent_directory)
    if not parent_directory:
        parent_directory = "."

    html_content += """
    <tr>
        <td><a href="{}">../</a></td>
        <td></td>
        <td></td>
    </tr>
    """.format(parent_directory)

    # Add each item in the directory to the HTML
    for item in items:
        item_path = os.path.join(directory_path, item)
        if os.path.isdir(item_path):
            item_link = item + '/'
        else:
            item_link = item

        last_modified = datetime.fromtimestamp(os.path.getmtime(item_path)).strftime('%Y-%m-%d %H:%M:%S')
        size = os.path.getsize(item_path) if not os.path.isdir(item_path) else '-'

        html_content += """
        <tr>
            <td><a href="{0}">{1}</a></td>
            <td>{2}</td>
            <td>{3}</td>
        </tr>
        """.format(item_path, item_link, last_modified, size)

    # Close the HTML document
    html_content += """
    </table>
</body>
</html>
"""

    # Write the HTML content to the standard_output, aka the pipe
    print(html_content)


# Script start
generate_directory_listing_html("www")
