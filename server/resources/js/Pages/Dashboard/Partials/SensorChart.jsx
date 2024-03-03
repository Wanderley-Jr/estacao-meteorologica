import { FormControl, MenuItem, Select } from "@mui/material";
import axios from "axios";
import { useEffect, useState } from "react";
import { LineChart } from "@mui/x-charts";

const sensors= {
    [a]: {
        title: "Temperatura do solo",
        unit: "°C",
        transform: (x) => x.toFixed(2)
    },
    soil_temperature: {
        title: "Temperatura do solo",
        unit: "°C",
        transform: (x) => x.toFixed(2)
    },
    soil_humidity: {
        title: "Umidade do solo",
        unit: "%",
        transform: (x) => x.toFixed(2)
    },
    air_temperature: {
        title: "Temperatura do ar",
        unit: "°C",
        transform: (x) => x.toFixed(2)
    },
    air_humidity: {
        title: "Umidade do ar",
        unit: "%",
        transform: (x) => x.toFixed(2)
    },
    pressure: {
        title: "Pressão atmosférica",
        unit: "hPa",
        transform: (x) => (x/100).toFixed(2)
    },
    rain: {
        title: "Chuva",
        unit: "%",
        transform: (x) => x.toFixed(2)
    },
    luminosity: {
        title: "Luminosidade",
        unit: "%",
        transform: (x) => x.toFixed(2)
    }
};

/**
 * @param {string} value
 * @returns {JSX.Element}
 */
export default function SensorChart(value) {
    const [option, setOption] = useState(0)
    const [data, setData] = useSensorData()

    return (
        <div className="bg-white w-fit shadow-sm sm:rounded-lg p-4">
            <ChartHeader
                options={[
                    "Hoje",
                    "Últimos 7 dias",
                    "Últimos 30 dias"
                ]}
                selected={option}
                onSelect={setOption}
                current={data?.at(-1).value ?? 0}
            />
            <Chart data={data ?? []} />
        </div>
    );
}

function useSensorData() {
    const [data, setData] = useState()

    // On mount, load all data from server
    useEffect(() => {
        axios.get('/sanctum/csrf-cookie')
            .then(() => axios.get(`/measurements`, {
                params: {
                    sensor: "soil_temperature"
                }
            }))
            .then(res => {
                setData(res.data.measurements)
            })
    })

    // Periodically update page with newer measurements only
    useEffect(() => {
        const timer = setInterval(async () => {
            if (!data) return
            const start = new Date(data.at(-1)["time"]*1000) // Most recent measurement
            axios.get(`/measurements`, {
                params: {
                    sensor: "soil_temperature",
                    start
                }
            }).then(res => {
                if (res.data.measurements.length) {
                    setData(data => [...data, ...res.data.measurements])
                }
            })
        }, 5_000)

        return () => clearInterval(timer)
    })

    return /** @type {const} */ ([data, setData])
}

/**
 * @param {object} props
 * @param {number} props.current
 * @param {number} props.average
 * @param {string[]} props.options
 * @param {string} props.selected
 * @param {(a: number) => void} props.onSelect
 */
function ChartHeader({ current, average = 0, onSelect, selected }) {
    return (
        <div className="flex justify-between items-center">
            <div className="grid grid-cols-2 gap-x-3 justify-between">
                <h1 className="col-span-2 font-semibold text-2xl text-gray-800 leading-tight">Temperatura do solo</h1>
                <span className="text-xl">Atual: {current}°C</span>
            </div>

            <FormControl size="small">
                <Select
                    variant="standard"
                    disableUnderline
                    id="select"
                    label="Visualizar"
                    value={selected}
                    onChange={(e) => onSelect(e.target.value)}
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
function Chart({ data }) {
    return (
        <LineChart
            width={750}
            height={500}
            xAxis={[{
                data: data.map(it => new Date(it.time*1000)),
                scaleType: "time",
            }]}
            series={[{
                data: data.map(it => it.value),
                showMark: ({ index }) => !index || index+1 == data.length
            }]}
            margin={{ top: 10, bottom: 50, left: 50, right: 10 }}
        />
    )
}
