<?php
    
    // configuration
    require("../includes/config.php");
    
    // if form was submitted
    if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        // validate submission
        
        // share symbol not provides
        if (empty($_POST["symbol"]))
        {
            apologize("You must provide the stock symbol.");
        }
        
        // number of shares to be sold not provided
        else if (empty($_POST["shares"]))
        {
            apologize("You must provide the number of shares you want to buy.");
        }
        
        // number of shares to be sold not a non-negative integer
        else if (preg_match("/^\d+$/", $_POST["shares"]) == false)
        {
            apologize("Number of shares must be a non-negative integer.");   
        }
        
        $rows = query("SELECT * FROM stocks WHERE id = ? AND symbol = ?", $_SESSION["id"], $_POST["symbol"]);
        
        // check if user owns any shares of stock to be sold
        if($rows === false)
        {
            apologize("You cannot sell a stock you don't own.");    
        }
        
        // check if user has the amount of stocks he/she wants to sell
        else if($rows[0]["shares"] < $_POST["shares"])
        {
            apologize("You don't have that much shares.");    
        }              
        
        // get user info from database
        $rows = query("SELECT * FROM users WHERE id = ?", $_SESSION["id"]);
                
        // cash available with user
        $cash = $rows[0]["cash"];
        
        // ensure stock symbol is in uppercase
        $symbol = strtoupper($_POST["symbol"]);
        
        // get stock info
        $stock = lookup($symbol);
        
        // get stock price
        $price = $stock["price"];
                       
        // update cash
        $cash = $cash + ($price * $_POST["shares"]);
        
        // update database
        $update_1 = query("UPDATE users SET cash = ? WHERE id = ?", $cash, $_SESSION["id"] );
        $update_2 = query("INSERT INTO stocks (id, symbol, shares) VALUES(?, ?, ?) ON DUPLICATE KEY UPDATE shares = shares - ?", $_SESSION["id"], $symbol, $_POST["shares"], $_POST["shares"]); 
        
        // if any one databse update fails
        if ($update_1 === false || $update_2 === false)
        {
            apologize("Could not update database.");
        }
        
        // else update history
        else
        {
            $update_3 = query("INSERT INTO history (id, action, symbol, shares, time, price) VALUES(?, ?, ?, ?, CURRENT_TIMESTAMP, ?)", $_SESSION["id"], "SOLD", $symbol, $_POST["shares"], $price);
            
            // if operation fails
            if($update_3 === false)
                apologize("Could not update history."); 
        }
        
        // back to portfolio
        redirect("/");
        
    }
    
    // if form was not submitted, render form
    else
    {
        render("sell_form.php", ["title" => "Sell Shares"]);
    }
    
?>
