status_codes = {
    400: "Bad Request",
    401: "Unauthorized",
    403: "Forbidden",
    404: "Not Found",
    405: "Method Not Allowed",
    408: "Request Timeout",
    409: "Conflict",
    410: "Gone",
    411: "Length Required",
    413: "Payload Too Large",
    414: "URI Too Long",
    415: "Unsupported Media Type",
    429: "Too Many Requests",
    500: "Internal Server Error",
    501: "Not Implemented",
    502: "Bad Gateway",
    503: "Service Unavailable",
    504: "Gateway Timeout",
    505: "HTTP Version Not Supported",
    507: "Insufficient Storage"
}

template = """<!DOCTYPE html>
<html>
    <head>
        <title>{code} Error Page</title>
    </head>
    <body>
        <h1>{code}</h1>
        <p>{message}</p>
    </body>
</html>
"""

for code, message in status_codes.items():
    filename = f"{code}.html"
    with open(filename, "w") as file:
        file.write(template.format(code=code, message=message))
    print(f"Created {filename}")
