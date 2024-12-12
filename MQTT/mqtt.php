<!DOCTYPE html>
<html lang="en">
<head>
    <title>Sensor Data and Register</title>
    <style>
        body {
            font-family: Times, sans-serif;
            background-color: #4a9dd9;
            margin: 100px;
        }
        h1 {
            color: #ffffff;
        }
        #chart_div {
            width: 100%;
            height: 800px;
            margin-top: 50px;
        }
    </style>
</head>
<body>
    <h1>Bjorn Lavik Sensor Data - Potentiometer Visualization</h1>
    <p>Insert data like this: https://bjornlavik.com/insert_potentiometer.php?adc_value=123</p>

    <?php
    // Enable error reporting
    error_reporting(E_ALL);
    ini_set('display_errors', 1);

    $servername = "localhost";
    $username = "u866338639_db_BjornLavik";
    $password = "lavikbDBpass1";
    $dbname = "u866338639_BjornLavik";

    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    // Get the sensor data (adc_value) and timestamp
    $sql_chart_data = "SELECT timestamp, adc_value FROM sensor_value WHERE id IS NOT NULL";
    $result_chart_data = $conn->query($sql_chart_data);

    // Check if there is data
    if ($result_chart_data->num_rows > 0) {
        // Prepare data for Google Chart
        $dataArray = [['Timestamp', 'Potentiometer Value']];
        while ($chart_row = $result_chart_data->fetch_assoc()) {
            $dataArray[] = [date('Y-m-d H:i:s', strtotime($chart_row['timestamp'])), (float)$chart_row['adc_value']];
        }
        $dataJson = json_encode($dataArray);
    } else {
        echo "No data found!";
        $dataJson = '[]';
    }

    $conn->close();
    ?>

    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
        google.charts.load('current', {'packages':['corechart']});
        google.charts.setOnLoadCallback(drawChart);

        function drawChart() {
            try {
                const data = google.visualization.arrayToDataTable(<?php echo $dataJson; ?>);

                if (data.getNumberOfRows() === 0) {
                    throw new Error("No data available for chart.");
                }

                var options = {
                    title: 'Potentiometer Value vs. Time',
                    vAxis: { title: 'Potentiometer Value' },
                    hAxis: { title: 'Timestamp', slantedText: true },
                    series: {0: { pointSize: 6 }},
                    curveType: 'function',
                    legend: { position: 'none' }
                };

                var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
                chart.draw(data, options);
            } catch (error) {
                console.error("Error drawing chart:", error);
            }
        }
    </script>

    <div id="chart_div"></div>
</body>
</html>
