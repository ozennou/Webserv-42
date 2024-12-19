#!/usr/bin/env python3
import cgi
import cgitb
import html
import os
import sys
import io
import secrets
from datetime import datetime, timedelta
from http.cookies import SimpleCookie

# Enable detailed error reporting
cgitb.enable()

try:
    # Check if cookie exists
    cookie_str = os.environ.get('cookie', '')
    is_logged_in = 'session=' in cookie_str
    
    # If it's a POST request, set new cookie
    is_login_attempt = os.environ.get('REQUEST_METHOD') == 'POST'
    if is_login_attempt:
        session_token = secrets.token_urlsafe(32)
        # Write cookie header manually with precise formatting
        sys.stdout.write(f"Set-Cookie: session={session_token}; Path=/; HttpOnly; Secure; SameSite=Lax\r\n")
    
    # Print Content-Type header
    sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n")
    # Empty line to separate headers from body
    sys.stdout.write("\r\n")
    sys.stdout.flush()
    
    # HTML template
    print("""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>Login Page</title>
    </head>
    <body>
    """)

    if is_logged_in:
        print("""
        <h1>Welcome Back!</h1>
        <p>You are already logged in.</p>
        <p>Your session cookie is present.</p>
        """)
    else:
        # Display the login button
        print("""
        <h1>Please Login</h1>
        <form method="post" action="">
            <input type="submit" value="Login">
        </form>
        """)
        if is_login_attempt:
            print("<p>Login successful! Cookie has been set.</p>")

    print("""
    </body>
    </html>
    """)

except Exception as e:
    # Print headers for error page
    sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n")
    sys.stdout.write("\r\n")
    sys.stdout.flush()
    
    # Print error information
    print("""
    <!DOCTYPE html>
    <html>
    <body>
    <h1>CGI Script Error</h1>
    <pre>
    Error: {}
    
    Environment Variables:
    """.format(html.escape(str(e))))
    
    for key, value in os.environ.items():
        print("{}: {}".format(html.escape(key), html.escape(str(value))))
    
    print("""
    </pre>
    </body>
    </html>
    """)