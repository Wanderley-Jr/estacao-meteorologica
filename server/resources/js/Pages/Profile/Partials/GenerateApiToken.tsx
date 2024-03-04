import { useState } from 'react';
import DangerButton from '@/Components/DangerButton';
import InputError from '@/Components/InputError';
import InputLabel from '@/Components/InputLabel';
import Modal from '@/Components/Modal';
import TextInput from '@/Components/TextInput';
import { useForm, router } from '@inertiajs/react';
import PrimaryButton from '@/Components/PrimaryButton';
import { Transition } from '@headlessui/react';

export type GenerateApiTokenProps = {
    className: string
    apiTokens: string[]
    token?: string
}

export default function GenerateApiToken({ apiTokens, token, className = '' }: GenerateApiTokenProps) {
    const [modalShown, setModalShown] = useState(true)

    const { data, setData, errors, post, reset, processing, recentlySuccessful } = useForm({
        token_name: ''
    });

    const createToken: React.FormEventHandler<HTMLFormElement> = e => {
        e.preventDefault();

        post(route('token.create'), {
            preserveScroll: true,
            preserveState: false,
            onSuccess: () => {
                reset()
            },
        });
    };

    const deleteToken = (name: string) => {
        router.delete(route('token.destroy', name), { 
            preserveScroll: true 
        })
    }

    return (
        <section className={`space-y-6 ${className}`}>
            <header>
                <h2 className="text-lg font-medium text-gray-900">Generate API Token</h2>
                <p className="mt-1 text-sm text-gray-600">Generate a token for interacting with the API</p>
            </header>

            {apiTokens.length > 0 && <div>
                <h3 className="text-base font-medium mb-2 text-gray-900">Generated tokens</h3>

                {apiTokens.map((token, i) => (
                    <div key={i} className="flex items-center justify-between p-2 bg-gray-100 shadow sm:rounded-lg">
                        <span className="text-lg font-medium text-gray-900">{token}</span>
                        <DangerButton onClick={() => deleteToken(token)}>Delete</DangerButton>
                    </div>)
                )}
            </div>}

            <form onSubmit={createToken} className="mt-6 space-y-6">
                <div>
                    <h3 className="text-base font-medium mb-2 text-gray-900">Create new token</h3>

                    <InputLabel htmlFor="token_name" value="Token Name" />

                    <TextInput
                        id="token_name"
                        value={data.token_name}
                        onChange={(e) => setData('token_name', e.target.value)}
                        className="mt-1 block w-full"
                        autoComplete="off"
                    />

                    <InputError message={errors.token_name} className="mt-2" />
                </div>

                <div className="flex items-center gap-4">
                    <PrimaryButton disabled={processing}>Create</PrimaryButton>

                    <Transition
                        show={recentlySuccessful}
                        enter="transition ease-in-out"
                        enterFrom="opacity-0"
                        leave="transition ease-in-out"
                        leaveTo="opacity-0"
                    >
                        <p className="text-sm text-gray-600">Created.</p>
                    </Transition>
                </div>
            </form>

            <Modal show={modalShown && token != undefined} onClose={() => setModalShown(false)}>
                <div className="p-6">
                    <h2 className="text-lg font-medium text-gray-900">
                        Token Created:
                    </h2>

                    <p className="mt-1 text-sm text-gray-600">
                        {token}
                    </p>

                    <p className="mt-1 text-sm text-gray-600">
                        Copy this token and save it somewhere. This will not be shown again.
                    </p>
                </div>
            </Modal>
        </section>
    );
}
