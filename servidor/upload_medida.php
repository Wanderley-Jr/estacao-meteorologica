<?php
    # Recebe os dados do esp32 e realiza o upload ao banco de dados.
    $data = json_decode(file_get_contents("php://input"), true);

    if (!isset($data["temperatura"]) || !isset($data["umidade"])) {
        http_response_code(400); // Malformed syntax
        echo file_get_contents("php://input");
        return;
    }

    $temperatura = $data["temperatura"];
    $umidade = $data["umidade"];

    $db = new mysqli("localhost", "root", "admin");
    $db->query("INSERT INTO estacao_metereologica.medidas(temperatura, umidade) VALUES ($temperatura, $umidade)");
    http_response_code(200); // Ou 201
?>
