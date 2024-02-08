<?php 
    function getDatabaseConnection() {
        require_once "config.php";
        mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
        $db = new mysqli($db_config["host"], $db_config["username"], $db_config["password"]);
        initializeDatabase($db);
        return $db;
    }

    function initializeDatabase(mysqli $db) {
        $db->query("CREATE DATABASE IF NOT EXISTS estacao_metereologica");
        $db->query("USE estacao_metereologica");
        initializeClientsTable($db);
        initializeSensorsTable($db);
        initializeMeasurementsTable($db);
    }

    function initializeMeasurementsTable(mysqli $db) {
        $db->query("CREATE TABLE IF NOT EXISTS Measurements (
            idMeasurement INT NOT NULL AUTO_INCREMENT,
            value DOUBLE NOT NULL,
            time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
            sensorId INT NOT NULL,
            clientId INT NOT NULL,
            PRIMARY KEY (idMeasurement),
            FOREIGN KEY (clientId) REFERENCES Clients(idClient),
            FOREIGN KEY (sensorId) REFERENCES Sensors(idSensor)
        )");
    }

    function initializeSensorsTable(mysqli $db) {
        $db->query("CREATE TABLE IF NOT EXISTS Sensors(
            idSensor INT NOT NULL AUTO_INCREMENT,
            name VARCHAR(255) NOT NULL,
            description VARCHAR(255) NOT NULL,
            PRIMARY KEY (idSensor),
            UNIQUE (name)
        )");

        $sensors = [
            "soilTemperature" => "Temperatura do solo",
            "soilHumidity" => "Umidade do solo",
            "airTemperature" => "Temperatura do ar",
            "airHumidity" => "Umidade do ar",
            "pressure" => "Pressão atmosférica",
            "rain" => "Chuva",
            "luminosity" => "Luminosidade"
        ];

        foreach($sensors as $key=>$value) {
            $stmt = $db->prepare("INSERT IGNORE INTO Sensors(name, description) VALUES (?, ?)");
            $stmt->bind_param("ss", $key, $value);
            $stmt->execute();
        }
    }

    function initializeClientsTable(mysqli $db) {
        $db->query("CREATE TABLE IF NOT EXISTS Clients(
            idClient INT NOT NULL AUTO_INCREMENT,
            name VARCHAR(255) NOT NULL,
            PRIMARY KEY (idClient)
        )");

        $db->query("INSERT IGNORE INTO Clients(idClient, name) VALUES (1, 'test')");
    }

    function findSensorId(mysqli $db, string $name): int {
        $stmt = $db->prepare("SELECT idSensor FROM Sensors WHERE name=?");
        $stmt->bind_param("s", $name);
        $stmt->execute();
        $stmt->bind_result($id);
        $stmt->fetch();
        return $id;
    }

    function insertMeasurement(mysqli $db, int $clientId, int $sensorId, float $value) {
        $stmt = $db->prepare("INSERT INTO Measurements(clientId, sensorId, value) VALUES (?, ?, ?)");
        $stmt->bind_param("iid", $clientId, $sensorId, $value);
        $stmt->execute();
    }
?>