#!/usr/bin/env python3
import cgi
import cgitb
import html
import os

# Enable CGI error reporting
cgitb.enable()

# Print the HTTP header
print("Content-Type: text/html; charset=utf-8")
print()

# Create an instance of FieldStorage 
form = cgi.FieldStorage()

# HTML template with embedded Python logic
print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Python CGI Example</title>
</head>
<body>
    <h1>Welcome to a Python CGI Script</h1>
""")

# Check if form was submitted
if "username" in form:
    # Safely escape user input to prevent XSS
    username = html.escape(form.getvalue('username'))
    print(f"<p>Hello, {username}! Welcome to our CGI application.</p>")

# Display a form
print("""
    <form method="post" action="">
        <label for="username">Enter your name:</label>
        <input type="text" name="username" id="username">
        <input type="submit" value="Submit">
    </form>

    <h2>Server Information</h2>
    <ul>
""")

# Print some server environment variables
env_vars = [
    'SERVER_SOFTWARE', 
    'SERVER_NAME', 
    'GATEWAY_INTERFACE', 
    'SERVER_PROTOCOL',
    'REQUEST_METHOD'
]

for var in env_vars:
    print(f"<li><strong>{var}:</strong> {html.escape(os.environ.get(var, 'Not set'))}</li>")

print("""
    </ul>
</body>
</html>
""")
