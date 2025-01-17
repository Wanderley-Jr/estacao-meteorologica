<?php

use App\Models\Sensor;
use App\Models\User;
use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration {
    /**
     * Run the migrations.
     */
    public function up(): void {
        Schema::create("measurements", function (Blueprint $table) {
            $table->id();
            $table->double("value");
            $table->timestamp("time")->default(DB::raw("CURRENT_TIMESTAMP"));
            $table
                ->foreignIdFor(User::class)
                ->constrained()
                ->cascadeOnDelete();
            $table
                ->foreignIdFor(Sensor::class)
                ->constrained()
                ->cascadeOnDelete();

            // Measurements for the same sensor must always have unique timestamps
            $table->unique(["sensor_id", "time"]);

            $table->timestamps();
        });
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void {
        Schema::dropIfExists("measurements");
    }
};
