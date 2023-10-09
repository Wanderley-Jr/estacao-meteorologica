CREATE DATABASE IF NOT EXISTS estacao_metereologica;

USE estacao_metereologica;

CREATE TABLE IF NOT EXISTS medidas (
    id_medida INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
    temperatura INT NOT NULL,
    umidade INT NOT NULL
);