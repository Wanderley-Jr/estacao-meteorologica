<?php
    http_response_code(500);
    require_once "../common/database.php";
    $sensor_data = json_decode(file_get_contents('php://input'), true);

    $stmt = $db->prepare(
        "INSERT INTO Measurements(soil_temperature, soil_humidity, air_temperature, air_humidity, pressure, rain, luminosity) 
        VALUES (?, ?, ?, ?, ?, ?, ?)"
    );

    $parameters = array("sTm", "sHm", "aTm", "aHm", "prs", "rai", "lum");
    foreach ($parameters as $param) {
        if (!isset($sensor_data[$param])) {
            http_response_code(400);
            die("Error: Missing measurement for: $param");
        }
    }

    $types = str_repeat("d", sizeof($parameters));
    $values = array_map(fn($param) => $sensor_data[$param], $parameters);
    $stmt->bind_param($types, ...$values);

    if ($stmt->execute()) {
        http_response_code(200); // Or 201
        echo "Data inserted into table successfully";
    } else {
        http_response_code(500);
        die("Error inserting data: " . $stmt->error);
    }
?>
