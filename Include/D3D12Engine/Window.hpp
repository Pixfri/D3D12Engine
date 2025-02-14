// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#ifndef DE_WINDOW_HPP
#define DE_WINDOW_HPP

#include <D3D12Engine/pch.hpp>

namespace D3D12Engine {
    class Application;
    
    class Window {
    public:
        Window(Application* app, HINSTANCE hInstance, float width, float height);
        ~Window();

        Window(const Window&) = delete;
        Window(Window&&) = delete;

        inline HWND GetHandle() const;

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;
    
    private:
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        HWND m_Handle;
        Application* m_App;
    };
}

#include <D3D12Engine/Window.inl>

#endif // DE_WINDOW_HPP
