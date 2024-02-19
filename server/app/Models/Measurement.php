<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

/**
 * App\Models\Measurement
 *
 * @property-read \App\Models\Sensor|null $sensor
 * @property-read \App\Models\User|null $user
 * @method static \Illuminate\Database\Eloquent\Builder|Measurement newModelQuery()
 * @method static \Illuminate\Database\Eloquent\Builder|Measurement newQuery()
 * @method static \Illuminate\Database\Eloquent\Builder|Measurement query()
 * @mixin \Eloquent
 */
class Measurement extends Model
{
    protected $fillable = ['value', 'time'];
    protected $casts = ['time' => 'timestamp'];

    public function user()
    {
        return $this->belongsTo(User::class);
    }

    public function sensor()
    {
        return $this->belongsTo(Sensor::class);
    }
}
