<?php
    require_once "../common/database.php";

    try {
        $db = getDatabaseConnection();
    }
    catch (error $err) {
        http_response_code(500);
        die("Error setting up database!");
    }

    $sensorData = json_decode(file_get_contents('php://input'), true);

    $sensorMappings = [
        "sTm" => "soilTemperature",
        "sHm" => "soilHumidity",
        "aTm" => "airTemperature",
        "aHm" => "airHumidity",
        "rai" => "rain",
        "lum" => "luminosity",
        "prs" => "pressure"
    ];

    foreach ($sensorData as $name=>$value) {
        if (!isset($sensorMappings[$name])) continue;

        $mapped = $sensorMappings[$name];
        $sensorId = findSensorId($db, $mapped);
        $client = 1;

        insertMeasurement($db, $client, $sensorId, $value);
    }

    http_response_code(200);
    echo "Data inserted into table successfully!";
?>
