// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#ifndef DE_APPLICATION_HPP
#define DE_APPLICATION_HPP


#include <D3D12Engine/pch.hpp>
#include <D3D12Engine/Core/StepTimer.hpp>
#include <D3D12Engine/Core/Window.hpp>
#include <D3D12Engine/RHI/Vertex.hpp>
#include <D3D12Engine/RHI/VertexBuffer.hpp>

#include <directxtk12/Keyboard.h>
#include <directxtk12/Mouse.h>


namespace D3D12Engine {
    constexpr uint32_t g_ScreenWidth = 1280;
    constexpr uint32_t g_ScreenHeight = 720;

    class Application {
    public:
        Application(HINSTANCE hInstance, bool useWarpDevice);
        ~Application();
        void Run();

        void OnInit();
        void Tick();
        void OnUpdate() const;
        void OnRender();
        void OnDestroy();

        void OnWindowSizeChanged(int width, int height);

    private:
        std::unique_ptr<Window> m_Window;
        static constexpr UINT FrameCount = 2;
        bool m_UseWarpDevice = false;
        float m_AspectRatio;

#ifdef DE_DEBUG
        ComPtr<ID3D12Debug> m_Debug;
        ComPtr<ID3D12Debug1> m_DxgiDebug;
        ComPtr<ID3D12InfoQueue> m_DxgiInfoQueue;
#endif
        
        CD3DX12_VIEWPORT m_Viewport;
        CD3DX12_RECT m_ScissorRect;
        ComPtr<IDXGISwapChain3> m_SwapChain;
        ComPtr<ID3D12Device> m_Device;
        ComPtr<ID3D12Resource> m_RenderTargets[FrameCount];
        ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
        ComPtr<ID3D12CommandQueue> m_CommandQueue;
        ComPtr<ID3D12RootSignature> m_RootSignature;
        ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
        ComPtr<ID3D12PipelineState> m_PipelineState;
        ComPtr<ID3D12GraphicsCommandList> m_CommandList;
        UINT m_RtvDescriptorSize;

        // App resources.
        std::unique_ptr<VertexBuffer<VertexPosColor>> m_VertexBuffer;

        // Application timer.
        StepTimer m_Timer;

        // Synchronisation objects.
        UINT m_FrameIndex;
        HANDLE m_FrameEvent;
        ComPtr<ID3D12Fence> m_Fence;
        UINT64 m_FenceValue;

        // Input objects.
        std::unique_ptr<DirectX::Mouse> m_Mouse;
        std::unique_ptr<DirectX::Keyboard> m_Keyboard;

        static void GetHardwareAdapter(IDXGIFactory1* pFactory,
                                       IDXGIAdapter1** ppAdapter,
                                       bool requestHighPerformanceAdapter = false);

        void LoadPipeline();
        void LoadAssets();
        void PopulateCommandList() const;
        void WaitForPreviousFrame();

        static std::wstring GetAssetFullPath(const std::wstring& assetName);
    };
}

#include <D3D12Engine/Application.inl>

#endif // DE_APPLICATION_HPP
