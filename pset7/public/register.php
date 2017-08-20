<?php
    
    // configuration
    require("../includes/config.php");

    // if form was submitted
    if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        // if username is empty
        if (empty($_POST["username"]))
            apologize("Please enter a Username.");
        
        // if username too long
        if (strlen($_POST["username"]) > 255)
            apologize("Username too long. Max length is 255 characters.");
        
        // if username already taken
        // query database for user
        $rows = query("SELECT * FROM users WHERE username = ?", $_POST["username"]);
        // if user found
        if (count($rows) != 0)
            apologize("Username already taken. Please choose a different username.");
        
        // if password field is empty
        if (empty($_POST["password"]))
            apologize("Please enter a password.");
        
        // if passwords don't match
        if (strcmp($_POST["password"], $_POST["confirmation"]) != 0)
            apologize("The two passwords don't match.");
        
        // if everything checks out, insert user in database
        if (query("INSERT INTO users (username, hash, cash) VALUES(?, ?, 10000.0000)", $_POST["username"], crypt($_POST["password"])) === false)
            apologize("Database Error! Try again.");
        
        // get id assigned to user
        $rows = query("SELECT LAST_INSERT_ID() AS id");
        $id = $rows[0]["id"];
        
        // set SESSION id
        $_SESSION["id"] = $id;
        
        // redirect to index.php
        redirect("/");
    }
    else
    {
        // else render form
        render("register_form.php", ["title" => "Register"]);
    }
    
?>
