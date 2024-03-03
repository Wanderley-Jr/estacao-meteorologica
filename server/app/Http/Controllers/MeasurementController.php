<?php

namespace App\Http\Controllers;

use App\Http\Controllers\Controller;
use App\Models\Measurement;
use App\Models\Sensor;
use Illuminate\Http\Request;
use Illuminate\Support\Carbon;
use Illuminate\Support\Facades\Log;
use Illuminate\Validation\Rule;
use Inertia\Inertia;

class MeasurementController extends Controller {
    public function get(Request $request): array {
        $request->validate([
            "sensor" => ["required", Rule::exists("sensors", "name")],
            "start" => "date",
            "end" => "date",
        ]);

        $sensor = Sensor::where("name", $request->input("sensor"))->first();
        $start = Carbon::parse($request->input("start", 0));
        $end = Carbon::parse($request->input("end"));

        $measurements = Measurement::whereBelongsTo($sensor)
            ->whereBetween("time", [$start, $end])
            ->orderBy("time")
            ->get(["value", "time"]);

        if ($measurements->isEmpty()) {
            return [];
        }

        return [
            "current" => $measurements->last()["value"],
            "average" => $measurements->avg("value"),
            "measurements" => $measurements->all(),
        ];
    }

    public function post(Request $request): void {
        $request->validate([
            "*.sensor" => ["required", Rule::exists("sensors", "name")],
            "*.value" => "required|numeric",
        ]);

        foreach ($request->all() as $res) {
            $sensor = Sensor::firstWhere("name", $res["sensor"]);
            $measurement = new Measurement(["value" => $res["value"]]);
            $measurement->user()->associate($request->user());
            $measurement->sensor()->associate($sensor);
            $measurement->save();
        }
    }
}
