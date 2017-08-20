<table class = "table table-striped">
    
    <thead>
        <tr>
            <th>Action</th>
            <th>Symbol</th>
            <th>Shares</th>
            <th>Time of Transcation</th>
            <th>Price</th>
        </tr>
    </thead>
    
    <tbody>
        <?php
            // print stock data
            foreach ($positions as $position)
            {
                print("<tr>");
                print("<td>" . "<div class = text-left>" . $position["action"] . "</div>" . "</td>");
                print("<td>" . "<div class = text-left>" . $position["symbol"] . "</div>" . "</td>");
                print("<td>" . "<div class = text-left>" . $position["shares"] . "</div>" . "</td>");
                print("<td>" . "<div class = text-left>" . $position["time"] . "</div>" . "</td>");
                print("<td>" . "<div class = text-left>" . "$ ". $position["price"] . "</div>" . "</td>");
                print("</tr>");
            }
        ?>
    </tbody>

</table>
