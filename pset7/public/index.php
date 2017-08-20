<?php

    // configuration
    require("../includes/config.php"); 

    // get user info from database
    $rows = query("SELECT * FROM users WHERE id = ?", $_SESSION["id"]);
    
    // cash available with user
    $cash = $rows[0]["cash"];
    
    // get user stock data from database
    $rows = query("SELECT * FROM stocks WHERE id = ?", $_SESSION["id"]);
    
    // array to hold stock data
    $positions = [];
    foreach ($rows as $row)
    {
        // lookup stock data
        $stock = lookup($row["symbol"]);
        
        // if stock data found
        if ($stock !== false)
        {
            // insert stock data
            $positions[] = ["name" => $stock["name"],
                            "price" => $stock["price"],
                            "shares" => $row["shares"],
                            "symbol" => $row["symbol"]
                           ];
        }
    }
    
    // render portfolio
    render("portfolio.php", ["cash" => $cash, "positions" => $positions, "title" => "Portfolio"]);

?>
