<!-- Mostra os itens da tabela "medidas" numa página -->

<?php
    $db = mysqli_connect("localhost", "root");
    
    $result = $db->query("SELECT * FROM estacao_metereologica.medidas");

    echo "<table>";
    echo "<th>Id</th> <th>Temperatura</th> <th>Umidade</th>";
    while ($row = $result->fetch_row()) {
        echo "<tr>";

        echo "<td>" . $row[0] . "</td>";
        echo "<td>" . $row[1] . "°C</td>";
        echo "<td>" . $row[2] . "%</td>";

        echo "</tr>";
    }
    echo "</table>";
?>
