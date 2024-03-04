<?php

namespace App\Models;

use Eloquent as Model;
use Illuminate\Database\Eloquent\Builder;
use Illuminate\Database\Eloquent\Relations\BelongsTo;
use Illuminate\Support\Carbon;

/**
 * App\Models\Measurement
 *
 * @property int $id
 * @property float $value
 * @property int $time
 * @property int $user_id
 * @property int $sensor_id
 * @property Carbon|null $created_at
 * @property Carbon|null $updated_at
 * @property-read \App\Models\Sensor $sensor
 * @property-read \App\Models\User $user
 * @method static Builder|Measurement newModelQuery()
 * @method static Builder|Measurement newQuery()
 * @method static Builder|Measurement query()
 * @method static Builder|Measurement whereCreatedAt($value)
 * @method static Builder|Measurement whereId($value)
 * @method static Builder|Measurement whereSensorId($value)
 * @method static Builder|Measurement whereTime($value)
 * @method static Builder|Measurement whereUpdatedAt($value)
 * @method static Builder|Measurement whereUserId($value)
 * @method static Builder|Measurement whereValue($value)
 * @mixin Eloquent
 */
class Measurement extends Model {
    protected $fillable = ["value", "time"];
    protected $casts = ["time" => "timestamp"];

    public function user(): BelongsTo {
        return $this->belongsTo(User::class);
    }

    public function sensor(): BelongsTo {
        return $this->belongsTo(Sensor::class);
    }
}
