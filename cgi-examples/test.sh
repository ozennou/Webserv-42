#!/bin/bash

# This script outputs a simple HTML page

# Content-type header (required for CGI scripts)
echo "Content-type: text/html"
echo ""

# HTML content
echo "<html>"
echo "<head><title>CGI Shell Script Example</title></head>"
echo "<body>"
echo "<h1>Hello, World!</h1>"
echo "<p>This is a simple CGI script written in Bash.</p>"

# Get environment variables from the CGI request
echo "<p>Request Method: $REQUEST_METHOD</p>"
echo "<p>Query String: $QUERY_STRING</p>"

# Display the current date and time
echo "<p>Current date and time: $(date)</p>"

# Display a message if a parameter was passed in the URL (optional)
if [ -n "$QUERY_STRING" ]; then
    echo "<p>You passed the following parameter: $QUERY_STRING</p>"
else
    echo "<p>No parameters were passed.</p>"
fi

echo "</body>"
echo "</html>"

