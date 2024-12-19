<!-- index.html -->
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>File Upload Form</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 600px;
            margin: 50px auto;
            padding: 20px;
        }
        .upload-form {
            background: #f9f9f9;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .form-group {
            margin-bottom: 15px;
        }
        .button {
            background: #4CAF50;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        .button:hover {
            background: #45a049;
        }
        .message {
            padding: 10px;
            margin-top: 10px;
            border-radius: 4px;
        }
        .success {
            background: #dff0d8;
            color: #3c763d;
        }
        .error {
            background: #f2dede;
            color: #a94442;
        }
    </style>
</head>
<body>
    <div class="upload-form">
        <h2>File Upload</h2>
        <form action="upload.php" method="POST" enctype="multipart/form-data">
            <div class="form-group">
                <input type="file" name="fileToUpload" id="fileToUpload" required>
            </div>
            <div class="form-group">
                <button type="submit" name="submit" class="button">Upload File</button>
            </div>
        </form>
    </div>
</body>
</html>

<?php
// upload.php

// Set the target directory for uploads
$target_dir = "/tmp";

// Create the uploads directory if it doesn't exist
if (!file_exists($target_dir)) {
    mkdir($target_dir, 0777, true);
}

// Get the file name and path
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
$uploadOk = 1;
$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));

// Check if file was actually uploaded
if(isset($_POST["submit"])) {
    if(isset($_FILES["fileToUpload"])) {
        // Check if file already exists
        if (file_exists($target_file)) {
            echo "<div class='message error'>Sorry, file already exists.</div>";
            $uploadOk = 0;
        }

        // Check file size (limited to 5MB)
        if ($_FILES["fileToUpload"]["size"] > 5000000) {
            echo "<div class='message error'>Sorry, your file is too large. Maximum size is 5MB.</div>";
            $uploadOk = 0;
        }

        // Allow only certain file formats
        $allowed_types = array('jpg', 'jpeg', 'png', 'gif', 'pdf', 'doc', 'docx');
        if (!in_array($imageFileType, $allowed_types)) {
            echo "<div class='message error'>Sorry, only JPG, JPEG, PNG, GIF, PDF, DOC & DOCX files are allowed.</div>";
            $uploadOk = 0;
        }

        // Check if $uploadOk is set to 0 by an error
        if ($uploadOk == 0) {
            echo "<div class='message error'>Your file was not uploaded.</div>";
        } else {
            // Try to upload file
            if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
                echo "<div class='message success'>The file ". htmlspecialchars(basename($_FILES["fileToUpload"]["name"])). " has been uploaded successfully.</div>";
            } else {
                echo "<div class='message error'>Sorry, there was an error uploading your file.</div>";
            }
        }
    } else {
        echo "<div class='message error'>No file was selected for upload.</div>";
    }
}
?>