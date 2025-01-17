<?php

use App\Http\Controllers\ProfileController;
use App\Http\Controllers\MeasurementController;
use Illuminate\Foundation\Application;
use Illuminate\Support\Facades\Route;
use Inertia\Inertia;

/*
|--------------------------------------------------------------------------
| Web Routes
|--------------------------------------------------------------------------
|
| Here is where you can register web routes for your application. These
| routes are loaded by the RouteServiceProvider within a group which
| contains the "web" middleware group. Now create something great!
|
*/

Route::get("/", function () {
    return Inertia::render("Welcome", [
        "canLogin" => Route::has("login"),
        "canRegister" => Route::has("register"),
        "laravelVersion" => Application::VERSION,
        "phpVersion" => PHP_VERSION,
    ]);
});

Route::get("/dashboard", function () {
    return Inertia::render("Dashboard/Dashboard");
})
    ->middleware(["auth", "verified"])
    ->name("dashboard");

Route::middleware("auth")->group(function () {
    Route::get("/profile", [ProfileController::class, "edit"])->name("profile.edit");

    Route::patch("/profile", [ProfileController::class, "update"])->name("profile.update");

    Route::delete("/profile", [ProfileController::class, "destroy"])->name("profile.destroy");

    Route::post("/profile/create-token", [ProfileController::class, "createToken"])->name("token.create");

    Route::delete("/profile/destroy-token/{tokenName}", [ProfileController::class, "destroyToken"])->name(
        "token.destroy"
    );
});

Route::middleware("auth")->group(function () {
    Route::get("/measurements", [MeasurementController::class, "get"]);
});

require __DIR__ . "/auth.php";
