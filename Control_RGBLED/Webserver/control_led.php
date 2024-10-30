<?php
// Check if a button was pressed and update the results.txt
if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    if (isset($_GET['led'])) {
        $led_status = ($_GET['led'] === 'on') ? "LED_ON" : "LED_OFF";
        file_put_contents("results.txt", $led_status);
    }
}

// Read the last LED state from results.txt
$last_led_state = file_get_contents("results.txt");
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Control LED</title>
</head>
<body>
    <h1>Control LED</h1>
   
    <p>Last LED state: <?php echo ($last_led_state === "LED_ON") ? "ON" : "OFF"; ?></p>
    
    <form method="get">
        <button type="submit" name="led" value="on">Turn LED ON</button>
        <button type="submit" name="led" value="off">Turn LED OFF</button>
    </form>
</body>
</html>
