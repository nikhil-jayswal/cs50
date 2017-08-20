<?php

    // configuration
    require("../includes/config.php");
    
    // if form was submitted
    if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        // validate submission
        
        // old password was  not provided
        if (empty($_POST["old_password"]))
        {
            apologize("You must provide your old password.");
        }
        
        // new password not provided
        else if (empty($_POST["new_password"]))
        {
            apologize("New password cannot be empty.");
        }
        
        // confirmation not provided
        else if (strcmp($_POST["new_password"], $_POST["confirm_password"]) != 0)
            apologize("New Password not confirmed."); 
        
        // change password
        $hash = crypt($_POST["new_password"]);
        
        // update user info in database
        $rows = query("UPDATE users SET hash = ? WHERE id = ?", $hash, $_SESSION["id"]);
        if ($rows === false)
        {
            apologize("Could not update password. Try again.");
        }
        
        redirect("/");  
    }
    
    // else show password change form
    else
    {
        render("password.php", ["title" => "Change your Password"]);
    }
?>
