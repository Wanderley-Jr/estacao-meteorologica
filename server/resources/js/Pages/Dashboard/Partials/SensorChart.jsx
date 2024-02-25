import { router, usePage } from "@inertiajs/react";
import { FormControl, InputLabel, MenuItem, Select } from "@mui/material";
import axios from "axios";
import { useEffect, useRef, useState } from "react";
import { CartesianGrid, Legend, Line, LineChart, ResponsiveContainer, Tooltip, XAxis, YAxis } from "recharts";

export default function SensorChart() {
    const [option, setOption] = useState(0)
    const [data, setData] = useState()

    // On mount, load all data from server
    useEffect(() => {
        console.log("AAA")
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
            const start = new Date(data.current.at(-1)["time"]*1000) // Most recent measurement
            axios.get(`/measurements`, {
                params: {
                    sensor: "soil_temperature",
                    start
                }
            }).then(res => {
                setData(data => [...data, ...res.data.measurements])
            })
        }, 60_000)

        return () => clearInterval(timer)
    })

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
                <span className="text-xl">Média: {average}°C</span>
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
    const dateFormat = (value, i) => {
        const d = new Date(value);

        return (
            d.getHours().toString().padStart(2, '0') + ":" +
            d.getMinutes().toString().padStart(2, '0') + ":" +
            d.getSeconds().toString().padStart(2, '0')
        )
    }

    return (
        <LineChart
            width={750}
            height={500}
            data={data}
            margin={{
                top: 20,
                right: 20+36.8,
                left: 20,
                bottom: 20,
            }}
        >
            <CartesianGrid strokeDasharray="3" />
            <XAxis dataKey="time" tickFormatter={dateFormat} scale="time" tickCount={5}/>
            <YAxis tickFormatter={v => v + "°C"} />
            <Tooltip/>
            <Legend/>
            <Line 
                name="Temperatura do solo"
                type="monotone" 
                dataKey="value" 
                stroke="#82ca9d"
            />
        </LineChart>
    )
}