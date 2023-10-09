<!-- Recebe as medidas via GET e as salva na database -->

<?php
    $temperatura = $_GET["temperatura"];
    $umidade = $_GET["umidade"];

    if (isset($temperatura) && isset($umidade)) {
        $db = new mysqli("localhost", "root");
        $db->query("INSERT INTO estacao_metereologica.medidas(temperatura, umidade) VALUES ($temperatura, $umidade)");
        http_response_code(200); // Ou 201
    }

    else {
        http_response_code(400); // Malformed syntax
    }
?>