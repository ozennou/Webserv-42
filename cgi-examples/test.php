#!/usr/bin/php-cgi
<?php
// Disable error display in production
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Set content type header
header('Content-Type: text/html; charset=utf-8');

// Simple session management
session_start();

// Function to sanitize input
function sanitize_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}

// Simple user authentication (mock example)
$valid_users = [
    'admin' => 'password123',
    'user' => 'user456'
];

// Handle login/logout
$action = isset($_GET['action']) ? $_GET['action'] : '';
$username = isset($_POST['username']) ? sanitize_input($_POST['username']) : '';
$password = isset($_POST['password']) ? sanitize_input($_POST['password']) : '';

if ($action === 'login') {
    if (isset($valid_users[$username]) && $valid_users[$username] === $password) {
        $_SESSION['logged_in'] = true;
        $_SESSION['username'] = $username;
    } else {
        $login_error = "Invalid username or password";
    }
} elseif ($action === 'logout') {
    session_destroy();
}

// Sample data processing
$todo_list = isset($_SESSION['todo_list']) ? $_SESSION['todo_list'] : [];

// Add new todo item
if (isset($_POST['new_todo']) && !empty($_POST['new_todo']) && isset($_SESSION['logged_in'])) {
    $new_todo = sanitize_input($_POST['new_todo']);
    $todo_list[] = $new_todo;
    $_SESSION['todo_list'] = $todo_list;
}

// Remove todo item
if (isset($_GET['remove']) && isset($_SESSION['logged_in'])) {
    $index = (int)$_GET['remove'];
    if (isset($todo_list[$index])) {
        unset($todo_list[$index]);
        $todo_list = array_values($todo_list);  // Reindex array
        $_SESSION['todo_list'] = $todo_list;
    }
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>PHP CGI Todo Manager</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            max-width: 600px; 
            margin: 0 auto; 
            padding: 20px; 
        }
        .error { 
            color: red; 
        }
        .todo-list { 
            margin-top: 20px; 
        }
        .todo-item { 
            display: flex; 
            justify-content: space-between; 
            padding: 10px; 
            border-bottom: 1px solid #eee; 
        }
    </style>
</head>
<body>
    <h1>Todo List Manager</h1>

    <?php if (!isset($_SESSION['logged_in'])): ?>
        <h2>Login</h2>
        <?php if (isset($login_error)): ?>
            <p class="error"><?php echo $login_error; ?></p>
        <?php endif; ?>
        <form method="post" action="?action=login">
            <label for="username">Username:</label>
            <input type="text" name="username" required><br>
            <label for="password">Password:</label>
            <input type="password" name="password" required><br>
            <input type="submit" value="Login">
        </form>

    <?php else: ?>
        <p>Welcome, <?php echo htmlspecialchars($_SESSION['username']); ?>! 
            <a href="?action=logout">Logout</a>
        </p>

        <form method="post">
            <input type="text" name="new_todo" placeholder="Enter new todo item" required>
            <input type="submit" value="Add Todo">
        </form>

        <div class="todo-list">
            <h2>Your Todo List</h2>
            <?php if (empty($todo_list)): ?>
                <p>No todo items yet.</p>
            <?php else: ?>
                <ul>
                    <?php foreach ($todo_list as $index => $todo): ?>
                        <li class="todo-item">
                            <?php echo htmlspecialchars($todo); ?>
                            <a href="?remove=<?php echo $index; ?>">Remove</a>
                        </li>
                    <?php endforeach; ?>
                </ul>
            <?php endif; ?>
        </div>

        <div>
            <h2>Server Information</h2>
            <ul>
                <?php
                $server_vars = [
                    'SERVER_SOFTWARE',
                    'SERVER_NAME',
                    'GATEWAY_INTERFACE',
                    'SERVER_PROTOCOL',
                    'REQUEST_METHOD'
                ];

                foreach ($server_vars as $var) {
                    echo "<li><strong>$var:</strong> " . 
                         htmlspecialchars(isset($_SERVER[$var]) ? $_SERVER[$var] : 'Not set') . 
                         "</li>";
                }
                ?>
            </ul>
        </div>
    <?php endif; ?>
</body>
</html>
