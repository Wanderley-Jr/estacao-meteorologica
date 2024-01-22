<!-- Mostra os itens da tabela "medidas" numa página -->

<?php
    require_once "../common/database.php";
    $result = $db->query("SELECT * FROM estacao_metereologica.Measurements");

    echo "<table border=\"1\">";
    echo "<thead>";
    echo "<tr>";
    echo "<th>Measurement ID</th>";
    echo "<th>Soil Temperature</th>";
    echo "<th>Soil Humidity</th>";
    echo "<th>Air Temperature</th>";
    echo "<th>Air Humidity</th>";
    echo "<th>Pressure</th>";
    echo "<th>Rain</th>";
    echo "<th>Luminosity</th>";
    echo "</tr>";
    echo "</thead>";
    echo "<tbody>";

    while ($row = $result->fetch_assoc()) {
        echo "<tr>";
        echo "<td>" . $row['measurement_id'] . "</td>";
        echo "<td>" . $row['soil_temperature'] . "</td>";
        echo "<td>" . $row['soil_humidity'] . "</td>";
        echo "<td>" . $row['air_temperature'] . "</td>";
        echo "<td>" . $row['air_humidity'] . "</td>";
        echo "<td>" . $row['pressure'] . "</td>";
        echo "<td>" . $row['rain'] . "</td>";
        echo "<td>" . $row['luminosity'] . "</td>";
        echo "</tr>";
    }

    echo "</tbody></table>";
?>
