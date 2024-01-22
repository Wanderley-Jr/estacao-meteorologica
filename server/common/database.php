<?php 
    function get_database_connection() {
        require_once "config.php";
        $db = new mysqli($db_config["host"], $db_config["username"], $db_config["password"]);
        if (!$db) {
            printf("Connection failed: %s\n", mysqli_connect_error());
            die();
        }
        return $db;
    }

    function create_database(mysqli $db) {
        $db->query("CREATE DATABASE IF NOT EXISTS estacao_metereologica");
        $db->query("USE estacao_metereologica");
        $db->query("CREATE TABLE IF NOT EXISTS Measurements (
            measurement_id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
            soil_temperature DOUBLE NOT NULL,
            soil_humidity DOUBLE NOT NULL,
            air_temperature DOUBLE NOT NULL,
            air_humidity DOUBLE NOT NULL,
            pressure DOUBLE NOT NULL,
            rain DOUBLE NOT NULL,
            luminosity DOUBLE NOT NULL
        )");

        if ($db->errno) {
            die("Query error: " . $db->error);
        }
    }

    $db = get_database_connection();
    create_database($db);
?>