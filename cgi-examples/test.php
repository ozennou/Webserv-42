<?php
// Simulate a database for storing comments (you can replace this with an actual database connection if needed)
$comments = [
    ['username' => 'John Doe', 'comment' => 'Great article! I learned a lot.'],
    ['username' => 'Jane Smith', 'comment' => 'Thanks for the insights. Very helpful!'],
];

// Handle comment submission
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['comment']) && !empty($_POST['comment'])) {
    // Validate and sanitize input
    $username = isset($_POST['username']) ? htmlspecialchars($_POST['username']) : 'Anonymous';
    $comment = htmlspecialchars($_POST['comment']);

    // Simulate adding the comment to the "database"
    $comments[] = ['username' => $username, 'comment' => $comment];
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Amazing PHP Article</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f7f7f7;
            color: #333;
            margin: 0;
            padding: 0;
        }
        header {
            background-color: #007bff;
            color: white;
            text-align: center;
            padding: 20px;
        }
        header h1 {
            font-size: 36px;
            margin: 0;
        }
        .content {
            max-width: 800px;
            margin: 30px auto;
            background-color: white;
            padding: 20px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        .content h2 {
            color: #007bff;
        }
        .content p {
            line-height: 1.6;
            font-size: 18px;
        }
        .comment-section {
            margin-top: 40px;
        }
        .comment-section h3 {
            color: #007bff;
            font-size: 24px;
        }
        .comment {
            background-color: #f9f9f9;
            padding: 10px;
            margin-bottom: 10px;
            border-left: 4px solid #007bff;
        }
        .comment p {
            margin: 5px 0;
        }
        .comment-form input, .comment-form textarea {
            width: 100%;
            padding: 10px;
            margin-bottom: 10px;
            border: 1px solid #ddd;
            font-size: 16px;
        }
        .comment-form button {
            background-color: #007bff;
            color: white;
            border: none;
            padding: 10px 20px;
            cursor: pointer;
            font-size: 16px;
        }
        .comment-form button:hover {
            background-color: #0056b3;
        }
    </style>
</head>
<body>

<header>
    <h1>Amazing PHP Article</h1>
    <p>Learn everything about PHP and how it can help you build dynamic websites!</p>
</header>

<div class="content">
    <h2>The Power of PHP in Modern Web Development</h2>
    <p><strong>Published on:</strong> <?php echo date('F j, Y'); ?></p>
    <p>PHP is one of the most widely used server-side scripting languages, and for good reason. It powers websites like Facebook, WordPress, and even Wikipedia. With its ability to interact with databases, manage sessions, and create dynamic content, PHP is an essential tool for any web developer. In this article, we’ll explore the power of PHP and how you can use it to enhance your website's functionality.</p>

    <h3>What is PHP?</h3>
    <p>PHP (Hypertext Preprocessor) is an open-source server-side scripting language primarily used for web development. Unlike client-side languages like JavaScript, PHP code is executed on the server, and the results are sent to the user's browser. This allows for dynamic content generation, such as personalized greetings, user logins, and more.</p>

    <h3>Why Use PHP?</h3>
    <p>PHP is extremely versatile. Whether you're building a simple blog, a content management system (CMS), or a complex e-commerce platform, PHP can handle it all. Here are some reasons why developers love PHP:</p>
    <ul>
        <li><strong>Easy to learn and use</strong></li>
        <li><strong>Works with all major databases</strong></li>
        <li><strong>Massive community support</strong></li>
        <li><strong>Open-source and free</strong></li>
    </ul>

    <h3>How PHP Works</h3>
    <p>PHP scripts are processed on the server before being sent to the browser as HTML. For example, when a user visits a page, PHP can retrieve data from a database, process it, and output the HTML that the user will see. This allows for a high degree of flexibility and customization for each user.</p>
</div>

<!-- Comment Section -->
<div class="comment-section">
    <h3>Leave a Comment</h3>

    <!-- Comment Form -->
    <form class="comment-form" method="POST" action="">
        <input type="text" name="username" placeholder="Your Name (Optional)" />
        <textarea name="comment" rows="5" placeholder="Write your comment..." required></textarea>
        <button type="submit">Submit Comment</button>
    </form>

    <!-- Display Comments -->
    <h3>Comments:</h3>
    <?php if (count($comments) > 0): ?>
        <?php foreach ($comments as $comment): ?>
            <div class="comment">
                <strong><?php echo htmlspecialchars($comment['username']); ?> says:</strong>
                <p><?php echo nl2br(htmlspecialchars($comment['comment'])); ?></p>
            </div>
        <?php endforeach; ?>
    <?php else: ?>
        <p>No comments yet. Be the first to leave one!</p>
    <?php endif; ?>
</div>

</body>
</html>
