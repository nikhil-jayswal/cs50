<ul class = "list-inline">
    <li><a href="quote.php">Quote</a></li>
    <li><a href="buy.php">Buy</a></li>
    <li><a href="sell.php">Sell</a></li>
    <li><a href="history.php">History</a></li>
    <li><a href="password_update.php"><strong>Change Password</strong></a></li>
    <li><a href="logout.php"><strong>Log Out</strong></a></li>
</ul>

<br/>

<?php
    // print cash
    print("<p class = \"text-center\"> CASH : ". "$ ". number_format($cash, 2) ."</p>");
?>

<table class = "table">
    
    <thead>
        <tr>
            <th>Symbol</th>
            <th>Name</th>
            <th>Shares</th>
            <th>Price</th>
            <th>TOTAL</th>
        </tr>
    </thead>
    
    <tbody>
        <?php
            // print stock data
            foreach ($positions as $position)
            {
                print("<tr>");
                print("<td>" . "<div class = text-left>" . $position["symbol"] . "</div>" . "</td>");
                print("<td>" . "<div class = text-left>" . $position["name"] . "</div>" . "</td>");
                print("<td>" . "<div class = text-left>" . $position["shares"] . "</div>" . "</td>");
                print("<td>" . "<div class = text-left>" . "$ " . $position["price"] . "</div>" . "</td>");
                print("<td>" . "<div class = text-left>" . "$ ". $position["shares"] * $position["price"] . "</div>" . "</td>");
                print("</tr>");
            }
        ?>
    </tbody>

</table>


