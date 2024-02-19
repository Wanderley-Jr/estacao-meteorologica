<?php

use App\Models\User;
use Illuminate\Database\Migrations\Migration;

return new class extends Migration
{
    /**
     * Run the migrations.
     */
    public function up(): void
    {
        // Create test user & generate API token
        User::factory()->create(['name' => 'test'])->createToken('test');
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void
    {}
};
