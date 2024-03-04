import { FormControl, MenuItem, Select } from "@mui/material";
import { useState } from "react";
import { LineChart } from "@mui/x-charts";
import useMeasurements from "./useMeasurements";

export type SensorType = keyof typeof sensors

const sensors = {
    soil_temperature: {
        title: "Temperatura do solo",
        unit: "°C",
        transform: (x: number) => parseFloat(x.toFixed(2))
    },
    soil_humidity: {
        title: "Umidade do solo",
        unit: "%",
        transform: (x: number) => parseFloat(x.toFixed(2))
    },
    air_temperature: {
        title: "Temperatura do ar",
        unit: "°C",
        transform: (x: number) => parseFloat(x.toFixed(2))
    },
    air_humidity: {
        title: "Umidade do ar",
        unit: "%",
        transform: (x: number) => parseFloat(x.toFixed(2))
    },
    pressure: {
        title: "Pressão atmosférica",
        unit: " hPa",
        transform: (x: number) => parseFloat((x/100).toFixed(2))
    },
    rain: {
        title: "Chuva",
        unit: "%",
        transform: (x: number) => parseFloat(x.toFixed(2))
    },
    luminosity: {
        title: "Luminosidade",
        unit: "%",
        transform: (x: number) => parseFloat(x.toFixed(2))
    }
}

type SensorChartProps = {
    type: SensorType
}

export default function SensorChart({ type }: SensorChartProps) {
    const [option, setOption] = useState(0)
    
    let history = 0
    if (option == 0) {
        history = 24*3600 // Last day
    }
    else if (option == 1) {
        history = 7*24*3600 // Last 7 days
    }
    else if (option == 2) {
        history = 30*24*3600 // Last 30 days
    }
    const data = useMeasurements(type, history)

    return (
        <div className="bg-white w-fit shadow-sm sm:rounded-lg p-4">
            <ChartHeader
                type={type}
                selected={option}
                onSelect={setOption}
                current={data?.at(-1)?.value ?? 0}
            />
            <Chart history={history} type={type} data={data ?? []} />
        </div>
    );
}

type ChartHeaderProps = {
    type: keyof typeof sensors,
    current: number,
    average?: number,
    selected: number,
    onSelect: (a: number) => void
}

function ChartHeader({ type, current, average = 0, onSelect, selected }: ChartHeaderProps) {
    return (
        <div className="flex justify-between items-center">
            <div className="grid grid-cols-2 gap-x-3 justify-between">
                <h1 className="col-span-2 font-semibold text-2xl text-gray-800 leading-tight">{sensors[type].title}</h1>
                <span className="text-xl">Atual: {sensors[type].transform(current)}{sensors[type].unit}</span>
            </div>

            <FormControl size="small">
                <Select
                    variant="standard"
                    disableUnderline
                    id="select"
                    label="Visualizar"
                    value={selected}
                    onChange={(e) => onSelect(parseInt(e.target.value as string))}
                >
                    <MenuItem value={0}>Último dia</MenuItem>
                    <MenuItem value={1}>Últimos 7 dias</MenuItem>
                    <MenuItem value={2}>Últimos 30 dias</MenuItem>
                </Select>
            </FormControl>
        </div>
    )
}

/**
 * @param {object} props
 * @param {{ time: number, value: number }[]} props.data
 */

type ChartProps = {
    type: keyof typeof sensors,
    history: number,
    data: Array<{ 
        time: number, 
        value: number 
    }>
}

function Chart({ type, data }: ChartProps) {
    // Computes max & min values for Y axis based on a percentage of the max and min values
    const maxY = sensors[type].transform(data.length ? data.map(it => it.value).reduce((a,b) => Math.max(a,b)) * 1.10 : 0)
    const minY = sensors[type].transform(data.length ? data.map(it => it.value).reduce((a,b) => Math.min(a,b)) * 0.90 : 0)

    return (
        <LineChart
            width={600}
            height={400}
            xAxis={[{
                data: data.map(it => new Date(it.time*1000)),
                scaleType: "time",
            }]}
            yAxis={[{ min: minY, max: maxY }]}
            series={[{
                data: data.map(it => sensors[type].transform(it.value)),
                showMark: ({ index }) => !index || index+1 == data.length
            }]}
            margin={{ top: 10, bottom: 50, left: 50, right: 10 }}
        />
    )
}
