<?php

namespace App\Http\Controllers;

use App\Http\Requests\ProfileUpdateRequest;
use App\Models\User;
use DB;
use Illuminate\Contracts\Auth\MustVerifyEmail;
use Illuminate\Http\RedirectResponse;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Auth;
use Illuminate\Support\Facades\Redirect;
use Inertia\Inertia;
use Inertia\Response;

class ProfileController extends Controller
{
    /**
     * Display the user's profile form.
     */
    public function edit(Request $request): Response
    {
        /** @var \App\Models\User */
        $user = $request->user();

        return Inertia::render('Profile/Edit', [
            'mustVerifyEmail' => $request->user() instanceof MustVerifyEmail,
            'status' => session('status'),
            'apiTokens' => $user->tokens->pluck('name')->toArray(),
            'token' => session('token')
        ]);
    }

    /**
     * Update the user's profile information.
     */
    public function update(ProfileUpdateRequest $request): RedirectResponse
    {
        $request->user()->fill($request->validated());

        if ($request->user()->isDirty('email')) {
            $request->user()->email_verified_at = null;
        }

        $request->user()->save();

        return Redirect::route('profile.edit');
    }

    /**
     * Delete the user's account.
     */
    public function destroy(Request $request): RedirectResponse
    {
        $request->validate([
            'password' => ['required', 'current_password'],
        ]);

        $user = $request->user();

        Auth::logout();

        $user->delete();

        $request->session()->invalidate();
        $request->session()->regenerateToken();

        return Redirect::to('/');
    }

    public function createToken(Request $request): RedirectResponse {
        $request->validate(["token_name" => "required"]);
        $name = $request->json()->getString('token_name');
        /** @var \App\Models\User */
        $user = $request->user();
        $token = $user->createToken($name)->plainTextToken;
        return back()->with('token', $token);
    }

    public function destroyToken(Request $request, string $tokenName): RedirectResponse {
        /** @var \App\Models\User */
        $user = $request->user();

        $token = $user->tokens()->getQuery()->where('name', $tokenName)->first();

        if ($token != null) {
            $token->delete();
        }

        return back();
    }
}
