<?php

namespace App\Models;

use Eloquent as Model;
use Illuminate\Database\Eloquent\Relations\HasMany;

/**
 * App\Models\Sensor
 *
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor newModelQuery()
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor newQuery()
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor query()
 * @property int $id
 * @property string $name
 * @property string $description
 * @property \Illuminate\Support\Carbon|null $created_at
 * @property \Illuminate\Support\Carbon|null $updated_at
 * @property-read \Illuminate\Database\Eloquent\Collection<int, \App\Models\Measurement> $measurement
 * @property-read int|null $measurement_count
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor whereCreatedAt($value)
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor whereDescription($value)
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor whereId($value)
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor whereName($value)
 * @method static \Illuminate\Database\Eloquent\Builder|Sensor whereUpdatedAt($value)
 * @mixin Model
 */
class Sensor extends Model {
    protected $fillable = ["name", "description"];

    public function measurement(): HasMany {
        return $this->hasMany(Measurement::class);
    }
}
