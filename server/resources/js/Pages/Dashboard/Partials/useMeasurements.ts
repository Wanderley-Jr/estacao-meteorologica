import { useEffect, useRef, useState } from "react"
import { SensorType } from "./SensorChart"
import axios from "axios"

export type MeasurementResponse = {
    measurements?: Measurements
}

export type Measurements = Array<{ 
    time: number, 
    value: number 
}>

export default function useMeasurements(type: SensorType, history: number) {
    const [data, setData] = useState<Measurements>()
    const fetchData = useRef<() => Promise<void>>()

    useEffect(() => {
        fetchData.current = async () => {
            const startTime = data?.at(-1)?.["time"] ?? Date.now()/1000 - history
            const startDate = new Date(startTime*1000)

            console.log(startDate)

            const { measurements } = (await axios.get<MeasurementResponse>("/measurements/", {
                params: {
                    sensor: type,
                    start: startDate
                }
            })).data

            if (measurements !== undefined) {
                setData([...(data ?? []), ...measurements])
            }
        }
    }, [type, data, history])

    useEffect(() => {
        console.log("Running use effect again with ")
        console.log(history)
    
        const task = (async () => {
            // Firstly authenticate axios session
            await axios.get('/sanctum/csrf-cookie')

            // Load initial data
            await fetchData.current?.()

            // After that, update data every once in a while
            return setInterval(() => {
                fetchData.current?.()
            }, 5_000)
        })()

        return () => {
            task.then(timer => clearInterval(timer))
        }
    }, [history])

    return data
}