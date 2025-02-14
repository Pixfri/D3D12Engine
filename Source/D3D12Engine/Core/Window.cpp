// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include <D3D12Engine/Core/Window.hpp>

#include <D3D12Engine/Application.hpp>

#include <directxtk12/Mouse.h>
#include <directxtk12/Keyboard.h>

namespace D3D12Engine {
    Window::Window(Application* app, const HINSTANCE hInstance, const float width, const float height)
        : m_App(app) {
        // Create a window class.
        WNDCLASSEX windowClass{};
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WndProc;
        windowClass.hInstance = hInstance;
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        windowClass.lpszClassName = L"D3D12EngineClass";
        RegisterClassEx(&windowClass);

        RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        // Create the window
        m_Handle = CreateWindow(
            windowClass.lpszClassName,
            L"D3D12 Engine <Direct3D 12>",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr, // No parent window
            nullptr, // No menus
            hInstance,
            m_App
        );

        ShowWindow(m_Handle, SW_NORMAL);
    }

    Window::~Window() {
        if (m_Handle) {
            DestroyWindow(m_Handle);
        }
    }

    LRESULT Window::WndProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) {
        static bool sizeMove = false;
        auto* app = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        switch (msg) {
        case WM_CREATE:
            {
                // Save the Application* passed in to CreateWindow.
                const auto pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
            }
            return 0;
        case WM_PAINT:
            {
                if (app) {
                    app->Tick();
                }
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_ACTIVATE:
        case WM_INPUT:
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_MOUSEHOVER:
            DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
            break;

        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            break;

        case WM_ENTERSIZEMOVE:
            sizeMove = true;
            break;

        case WM_EXITSIZEMOVE:
            {
                sizeMove = false;
                if (app) {
                    RECT rc;
                    GetClientRect(hWnd, &rc);

                    app->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
                }
                break;
            }
        default:
            break;
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

}
