#!/usr/bin/env python3
import cgi
import cgitb
import html
import os
import sys
import io

# Enable detailed error reporting
cgitb.enable()

# Print the HTTP header first
print("Content-Type: text/html; charset=utf-8\r")
print("\r")

# Debugging function
def print_debug_info(error_message):
    print("""
    <!DOCTYPE html>
    <html>
    <body>
    <h1>CGI Script Error</h1>
    <pre>
    Error: {}
    
    Environment Variables:
    """.format(html.escape(str(error_message))))
    
    for key, value in os.environ.items():
        print("{}: {}".format(html.escape(key), html.escape(str(value))))
    
    print("""
    </pre>
    </body>
    </html>
    """)
    sys.exit(1)

try:
    # Check content length
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    
    # Read POST data manually if needed
    if content_length > 0:
        post_data = sys.stdin.read(content_length)
        # Simulate FieldStorage with manual input
        form = cgi.FieldStorage(
            fp=io.StringIO(post_data),
            environ={'REQUEST_METHOD':'POST', 'CONTENT_TYPE': os.environ.get('CONTENT_TYPE', '')}
        )
    else:
        # Fallback to standard method
        form = cgi.FieldStorage()

    # HTML template with embedded Python logic
    print("""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>CGI Form Handler</title>
    </head>
    <body>
        <h1>CGI Form Submission</h1>
    """)

    # Check if form was submitted
    if form:
        print("<h2>Submitted Data:</h2>")
        print("<ul>")
        
        # Iterate through all form fields
        for key in form.keys():
            # Get the value and escape it to prevent XSS
            value = html.escape(form.getvalue(key))
            print(f"<li><strong>{html.escape(key)}:</strong> {value}</li>")
        
        print("</ul>")

    # Display the form
    print("""
        <form method="post" action="/test.py">
            <div>
                <label for="name">Name:</label>
                <input type="text" id="name" name="name" required>
            </div>
            <div>
                <label for="email">Email:</label>
                <input type="email" id="email" name="email">
            </div>
            <div>
                <label for="message">Message:</label>
                <textarea id="message" name="message" rows="4"></textarea>
            </div>
            <div>
                <input type="submit" value="Submit">
            </div>
        </form>

        <h2>Environment Information</h2>
        <pre>
    """)

    # Print environment variables
    for key, value in os.environ.items():
        print(f"{html.escape(key)}: {html.escape(str(value))}")

    print("""
        </pre>
    </body>
    </html>
    """)

except Exception as e:
    # Catch-all error handling
    print_debug_info(e)