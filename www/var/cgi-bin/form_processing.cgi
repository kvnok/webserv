#!/usr/bin/env python3
import cgi
form = cgi.FieldStorage()           # parse form data
print('Content-type: text/html\n')  # header plus blank line
print('<title>Reply Page</title>')  # html reply page
if 'user' in form:
	print('<h1>Hello, ' + form['user'].value + '!</h1>')