<?php
    
    // files required
    require("../includes/functions.php");
    
    // if form was submitted
    if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        // validate form submission
        // if stock symbol provided
        if (!empty($_POST["symbol"]))
        {
            // convert to uppercase
            // lookup stock's latest price
            $stock = lookup(strtoupper($_POST["symbol"]));
            
            if($stock === false)
                apologize("Could not find relevant data. Try Again!"); 
            else
                render("stock_price.php",$stock);   
        }
        else
            apologize("Please provide a stock name.");
    }    
    
    // if form was not submitted
    else
    {
        // ask for stock symbol
        render("stock_symbol.php", ["title" => "Get Quote"]);
    }
?>


