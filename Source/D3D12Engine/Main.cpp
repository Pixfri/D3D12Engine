// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include <D3D12Engine/pch.hpp>
#include <D3D12Engine/Application.hpp>

#include <iostream>
#include <stdexcept>

int WINAPI WinMain(const HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/) {
    D3D12Engine::Application app(hInstance, false);
    try {
        app.Run();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}