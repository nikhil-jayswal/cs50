<?php
    
    // configuration
    require("../includes/config.php");
    
    // if form was submitted
    if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        // validate submission
        
        // share symbol was  not provided
        if (empty($_POST["symbol"]))
        {
            apologize("You must provide the stock symbol.");
        }
        
        // number of shares to be bought was not provided
        else if (empty($_POST["shares"]))
        {
            apologize("You must provide the number of shares you want to buy.");
        }
        
        // number of shares to be bought is not a non-negative integer
        else if (preg_match("/^\d+$/", $_POST["shares"]) == false)
        {
            apologize("Number of shares must be a non-negative integer.");   
        }
        
        // get user info from database
        $rows = query("SELECT * FROM users WHERE id = ?", $_SESSION["id"]);
      
        
        // cash available with the user
        $cash = $rows[0]["cash"];
        
        // ensure stock symbol is in uppercase
        $symbol = strtoupper($_POST["symbol"]);
        
        // lookup stock data
        $stock = lookup($symbol);
        
        // check if stock symbol is legit or data was returned
        if ($stock == false)
        {
            apologize("Something went wrong. Please check your stock symbol and try again.");
        }
               
        // get stock price
        $price = $stock["price"];
        
        // check if user has enough cash
        if ($price * $_POST["shares"] > $cash)
        {
            apologize("You do not have enough cash to carry out the transaction.");            
        }
        
        // update cash
        $cash = $cash - ($price * $_POST["shares"]);
        
        // update databases
        $update_1 = query("UPDATE users SET cash = ? WHERE id = ?", $cash, $_SESSION["id"] );
        $update_2 = query("INSERT INTO stocks (id, symbol, shares) VALUES(?, ?, ?) ON DUPLICATE KEY UPDATE shares = shares + ?", $_SESSION["id"], $symbol, $_POST["shares"], $_POST["shares"]); 
                
        // if any one database update fails
        if ($update_1 === false || $update_2 === false)
        {
            apologize("Could not carry out the transaction. Try again");
        }
        
        // else update history
        else
        {
            $update_3 = query("INSERT INTO history (id, action, symbol, shares, time, price) VALUES(?, ?, ?, ?, CURRENT_TIMESTAMP, ?)", $_SESSION["id"], "BOUGHT", $symbol, $_POST["shares"], $price);
            
            if($update_3 === false)
                apologize("Could not update history."); 
        }
        
        // back to portfolio
        redirect("/");
        
    }
    
    // if form was not submitted, render form
    else
    {
        render("buy_form.php", ["title" => "Buy Shares"]);
    }
    
?>
