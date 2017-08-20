<?php

    require(__DIR__ . "/../includes/config.php");

    // numerically indexed array of places
    $places = [];
    
    // initialize variables
    $var = [];
    $count = 0;
        
    // go below to the else block first
    
    // if there is no comma in $_GET["geo"]
    if (strpos($_GET["geo"], ",") === false)
    {
        // echo "IF \n";
        $geo = $_GET["geo"];
        $var = explode("+", $geo);
        $count = count($var);
        for($i = 0; $i < $count; $i++)
            $var[$i] = trim($var[$i]);    
    }
    
    else
    {
        // echo "ELSE \n";
        // simplify $_GET["geo"] - convert + to space
        $geo = urldecode($_GET["geo"]);
    
        // extract various search parameters separated by commas and remove
        // unwanted whitespace
        // NOTE: won't work if $_GET["geo"] = "Cambridge+MA" as there is no comma
        // See the if condition above.
        $var = explode(",", $geo);
        $count = count($var);
        for($i = 0; $i < $count; $i++)
            $var[$i] = trim($var[$i]);
    }
        
    // if $var contains a single numeric string - postal code 
    if ($count == 1)
    {
        if (is_numeric($var[0]))
        {
            $rows = query("SELECT * FROM places WHERE MATCH (postal_code) AGAINST (?)", $var[0]);
            array_push($places, $rows[0]);
        }
    }
    
    // I am assuming that the user will provide input in the correct order.
    else if ($count >= 2)
    {
        $rows = query("SELECT * FROM places WHERE MATCH (place_name) AGAINST (?) AND (admin_name1 LIKE ? OR admin_code1 LIKE ?)", $var[0], $var[1], $var[1]);
        for ($i = 0; $i < count($rows); $i++)
            array_push($places, $rows[$i]);    
    }
    
  
    // output places as JSON (pretty-printed for debugging convenience)
    header("Content-type: application/json");
    print(json_encode($places, JSON_PRETTY_PRINT));

?>
