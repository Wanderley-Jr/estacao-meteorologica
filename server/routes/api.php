<?php

use App\Models\Measurement;
use App\Models\Sensor;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| API Routes
|--------------------------------------------------------------------------
|
| Here is where you can register API routes for your application. These
| routes are loaded by the RouteServiceProvider and all of them will
| be assigned to the "api" middleware group. Make something great!
|
*/

Route::middleware('auth:sanctum')->post('/insert_measurement', function (Request $request) {
    $json = $request->json();

    $sensorMappings = [
        "sTm" => "soil_temperature",
        "sHm" => "soil_humidity",
        "aTm" => "air_temperature",
        "aHm" => "air_humidity",
        "rai" => "rain",
        "lum" => "luminosity",
        "prs" => "pressure",
    ];

    $sensorName = $sensorMappings[$json->getString('name')];
    $sensor = Sensor::query()->where('name', $sensorName)->first();
    $sensorValue = $json->getInt('value');

    $measurement = new Measurement(['value' => $sensorValue]);
    $measurement->user()->associate($request->user());
    $measurement->sensor()->associate($sensor);
    $measurement->save();
    
    return "success";
});
