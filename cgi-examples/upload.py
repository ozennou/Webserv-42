#!/usr/bin/env python3
import cgi
import cgitb
import os
import html
import sys
import io
import base64

# Enable detailed error reporting
cgitb.enable()

# Print the HTTP header first
print("Content-Type: text/html; charset=utf-8\r")
print("\r")

# Define the upload directory
UPLOAD_DIR = '/Users/mozennou/Desktop/Webserv-42/cgi-examples/uploads'

# Ensure the upload directory exists
if not os.path.exists(UPLOAD_DIR):
    os.makedirs(UPLOAD_DIR)

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
        <title>CGI Binary File Upload</title>
    </head>
    <body>
        <h1>CGI Binary File Upload</h1>
    """)

    # Check if form was submitted
    if form:
        print("<h2>Submitted Data:</h2>")
        print("<ul>")
        
        # Iterate through all form fields
        for key in form.keys():
            # Check if the field is a file
            field = form[key]
            if field.filename:
                # Save the uploaded file
                filename = os.path.basename(field.filename)
                filepath = os.path.join(UPLOAD_DIR, filename)
                with open(filepath, 'wb') as f:
                    f.write(field.file.read())
                print(f"<li><strong>File uploaded:</strong> {html.escape(filename)}</li>")
                
                # Optionally, encode the file to base64 for display
                with open(filepath, 'rb') as f:
                    file_content = f.read()
                    base64_content = base64.b64encode(file_content).decode('utf-8')

                print(f"""<li><strong>File preview (Base64 encoded):</strong><pre>{html.escape(base64_content)}</pre></li>""")
                
                # Provide a link to download the file
                download_link = f"/uploads/{filename}"
                print(f"""<li><strong>Download the file:</strong> <a href="{download_link}">Click here to download {html.escape(filename)}</a></li>""")

            else:
                # Handle non-file fields
                value = html.escape(field.value)
                print(f"<li><strong>{html.escape(key)}:</strong> {value}</li>")
        
        print("</ul>")

    # Display the form
    print("""
        <h2>Upload a Binary File</h2>
        <form method="post" action="/upload.py" enctype="multipart/form-data">
            <div>
                <label for="name">Name:</label>
                <input type="text" id="name" name="name" required>
            </div>
            <div>
                <label for="file">Select a binary file to upload:</label>
                <input type="file" id="file" name="file" required>
            </div>
            <div>
                <input type="submit" value="Upload">
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
