<?php
    
    // configuration
    require("../includes/config.php");
    
    $positions = query("SELECT * FROM history where id = ?", $_SESSION["id"]);
        
    // render history
    if ($positions != false)
        render("history_table.php", ["positions" => $positions, "title" => "Transaction History"]);
        
    else
    {
        apologize("Transaction history does not exist.");
    }
?>
