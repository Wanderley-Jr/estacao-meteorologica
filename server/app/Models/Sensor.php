<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

/**
 * App\Models\Sensor
 *
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor newModelQuery()
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor newQuery()
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor query()
 * @mixin \Eloquent
 */
class Sensor extends Model
{
    protected $fillable = ['name', 'description'];

    public function measurement() {
        return $this->hasMany(Measurement::class);
    }
}
