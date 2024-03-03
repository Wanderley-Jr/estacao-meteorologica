<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Schema;

return new class extends Migration {
    /**
     * Run the migrations.
     */
    public function up(): void {
        Schema::create("sensors", function (Blueprint $table) {
            $table->id();
            $table->string("name")->unique();
            $table->string("description");
            $table->timestamps();
        });

        DB::table("sensors")->insert([
            [
                "name" => "soil_temperature",
                "description" => "Temperatura do solo",
            ],
            ["name" => "soil_humidity", "description" => "Umidade do solo"],
            ["name" => "air_temperature", "description" => "Temperatura do ar"],
            ["name" => "air_humidity", "description" => "Umidade do ar"],
            ["name" => "pressure", "description" => "Pressão atmosférica"],
            ["name" => "rain", "description" => "Chuva"],
            ["name" => "luminosity", "description" => "Luminosidade"],
        ]);
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void {
        Schema::dropIfExists("sensors");
    }
};
