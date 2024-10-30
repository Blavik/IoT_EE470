<!DOCTYPE html>
<html>
<head>
    <title>RGB Control</title>
</head>
<body>
    <h1>RGB LED Control</h1>
    <form method="POST">
        <label for="red">Red:</label>
        <input type="range" id="red" name="red" min="0" max="255" value="0"><br>
        <label for="green">Green:</label>
        <input type="range" id="green" name="green" min="0" max="255" value="0"><br>
        <label for="blue">Blue:</label>
        <input type="range" id="blue" name="blue" min="0" max="255" value="0"><br><br>
        <button type="submit">Set RGB</button>
    </form>

    <?php
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $red = $_POST['red'] ?? 0;
        $green = $_POST['green'] ?? 0;
        $blue = $_POST['blue'] ?? 0;

        $file = fopen("rgb_values.txt", "w");
        fwrite($file, "$red,$green,$blue");
        fclose($file);

        echo "<p>RGB values set to R: $red, G: $green, B: $blue</p>";
    }
    ?>
</body>
</html>
