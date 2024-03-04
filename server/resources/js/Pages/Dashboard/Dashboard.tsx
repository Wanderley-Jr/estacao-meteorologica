import AuthenticatedLayout from '@/Layouts/AuthenticatedLayout';
import { Head } from '@inertiajs/react';
import SensorChart from './Partials/SensorChart';
import { PageProps } from '@/types';

export default function Dashboard({ auth }: PageProps) {
    return (
        <AuthenticatedLayout
            user={auth.user}
            header={<h2 className="font-semibold text-xl text-gray-800 leading-tight">Dashboard</h2>}
        >
            <Head title="Dashboard" />

            <div className="grid gap-8 py-12 grid-cols-2 place-items-center">
                <SensorChart type="air_temperature"/>
                <SensorChart type="air_humidity"/>
                <SensorChart type="soil_temperature"/>
                <SensorChart type="soil_humidity"/>
                <SensorChart type="luminosity"/>
                <SensorChart type="rain"/>
                <SensorChart type="pressure"/>
            </div>
        </AuthenticatedLayout>
    );
}
