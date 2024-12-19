#!/usr/bin/env python3
import cgi
import random
import os
from http.cookies import SimpleCookie

# Helper function to get cookies from the environment variable
def get_cookie(cookie_name):
    cookies = os.environ.get("HTTP_COOKIE")  # Get all cookies from the environment
    if cookies:
        cookie = SimpleCookie(cookies)  # Parse the cookie header
        if cookie_name in cookie:
            return cookie[cookie_name].value  # Return only the value of the cookie
    return None

# Helper function to set a cookie
def set_cookie(cookie_name, cookie_value):
    print(f"Set-Cookie: {cookie_name}={cookie_value}; Path=/\r")  # Set the cookie header

# Start HTML content
print("Content-type: text/html\r")
cookie = os.getenv('cookie')

# Check if the secret_number cookie exists
if cookie is None:
    # If there's no cookie, generate a new secret number
    secret_number = random.randint(1, 100)
    set_cookie('SECRET_NUMBER', secret_number)  # Set the secret number in the cookie
else:
    # If cookie exists, extract secret_number from the cookie value
    try:
        cookie_value = cookie.split('=')  # Split cookie into key-value pair
        if len(cookie_value) == 2 and cookie_value[0] == 'secret_number':
            secret_number = int(cookie_value[1])  # Get the secret number from the cookie
        else:
            raise ValueError("Invalid cookie format")
    except (ValueError, IndexError):
        # If cookie is invalid or corrupted, generate a new secret number
        secret_number = random.randint(1, 100)
        set_cookie('SECRET_NUMBER', secret_number)
# Try to get the secret number from the session cookie

# Convert secret_number back to an integer for comparison
try:
    secret_number = int(secret_number)
except ValueError:
    secret_number = random.randint(1, 100)  # If conversion fails, reset to a new number

# Get the player's guess from the form
form = cgi.FieldStorage()
guess = None
message = ""

if "guess" in form:
    try:
        guess = int(form["guess"].value)
    except ValueError:
        guess = None

# If there's a guess, check if it's correct
if guess is not None:
    if guess < secret_number:
        message = f"Your guess of {guess} is too low. Try again!"
    elif guess > secret_number:
        message = f"Your guess of {guess} is too high. Try again!"
    else:
        message = f"Congratulations! {guess} is the correct number!"
        # Once the player guesses correctly, reset the secret number
        secret_number = random.randint(1, 100)
        set_cookie('SECRET_NUMBER', secret_number)  # Reset the secret number in the cookie
else:
    message = "Please enter a number between 1 and 100."
print("\r")
print("<html><head><title>Guess the Number Game</title></head><body>")
# HTML form to submit the guess
print("<h1>Guess the Number Game</h1>")
print("<p>I'm thinking of a number between 1 and 100. Can you guess it?</p>")
print(f"<p>{message}</p>")
print("""
<form method="POST" action="game.py">
    <label for="guess">Your guess (1-100):</label>
    <input type="number" id="guess" name="guess" min="1" max="100" required>
    <input type="submit" value="Submit Guess">
</form>
</body></html>
""")
