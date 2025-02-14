// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include <D3D12Engine/Application.hpp>

#include <D3D12Engine/DxUtils.hpp>

namespace D3D12Engine {
    Application::Application(const HINSTANCE hInstance, const bool useWarpDevice)
        : m_UseWarpDevice(useWarpDevice),
          m_AspectRatio(static_cast<float>(g_ScreenWidth) / static_cast<float>(g_ScreenHeight)),
          m_Viewport(0.0f, 0.0f, g_ScreenWidth, g_ScreenHeight),
          m_ScissorRect(0, 0, g_ScreenWidth, g_ScreenHeight),
          m_RtvDescriptorSize(0),
          m_FrameIndex(0) {
        m_Window = std::make_unique<Window>(this, hInstance, g_ScreenWidth, g_ScreenHeight);
        OnInit();
    }

    Application::~Application() = default;

    void Application::Run() {
        HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
        if (FAILED(hr)) {
            return;
        }

        // Parse the command line parameters
        int argc;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

        LocalFree(argv);

        // Main message loop
        MSG msg = {};
        while (msg.message != WM_QUIT) {
            // Process any message in the queue
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        OnDestroy();
        CoUninitialize();
    }

    void Application::OnInit() {
        // Create input devices
        m_Keyboard = std::make_unique<DirectX::Keyboard>();
        m_Mouse = std::make_unique<DirectX::Mouse>();
        m_Mouse->SetWindow(m_Window->GetHandle());
        LoadPipeline();
        LoadAssets();
    }

    void Application::Tick() {
        m_Timer.Tick([&]() {
            OnUpdate();
        });

        OnRender();
    }

    void Application::OnUpdate() const {
        // Will be used by the camera.
        const auto kb = m_Keyboard->GetState();
        (void)(kb);
    }

    void Application::OnRender() {
        // Record all the commands we need to render the scene into the command list.
        PopulateCommandList();

        // Execute the command list.
        ID3D12CommandList* ppCommandLists[] = {m_CommandList.Get()};
        m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // Present the frame.
        ThrowIfFailed(m_SwapChain->Present(1, 0));

        WaitForPreviousFrame();
    }

    void Application::OnDestroy() {
        WaitForPreviousFrame();

        CloseHandle(m_FrameEvent);
    }

    void Application::LoadPipeline() {
        ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

        UINT dxgiFactoryFlags = 0;

#ifdef DE_DEBUG
        {
            ComPtr<ID3D12Debug> spDebugController0;
            ComPtr<ID3D12Debug1> spDebugController1;
            ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0)));
            spDebugController0->EnableDebugLayer();
            ThrowIfFailed(spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1)));
            spDebugController1->SetEnableGPUBasedValidation(true);

            // Enable additional debug layerS.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif

        ComPtr<IDXGIFactory4> factory;

        ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)), "Failed to create DXGI factory.");

        if (m_UseWarpDevice) {
            ComPtr<IDXGIAdapter> warpAdapter;
            ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)), "Failed to create warp adapter");

            ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)), "Failed to create device.");
        } else {
            ComPtr<IDXGIAdapter1> hardwareAdapter;
            GetHardwareAdapter(factory.Get(), &hardwareAdapter);

            ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
        }
        
        // Describe and create the command queue.
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)), "Failed to create command queue.");

        // Describe and create the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = FrameCount;
        swapChainDesc.Width = g_ScreenWidth;
        swapChainDesc.Height = g_ScreenHeight;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain;

        ThrowIfFailed(factory->CreateSwapChainForHwnd(
            m_CommandQueue.Get(), // Swap chain needs the queue so that it can force a flush on it.
            m_Window->GetHandle(),
            &swapChainDesc,
            nullptr, nullptr,
            &swapChain), "Failed to create swap chain.");

        // This app doesn't support fullscreen transitions
        ThrowIfFailed(factory->MakeWindowAssociation(m_Window->GetHandle(), DXGI_MWA_NO_ALT_ENTER), "Failed to make window association");

        ThrowIfFailed(swapChain.As(&m_SwapChain), "Failed to get swap chain.");
        m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

        // Create descriptor heaps.
        {
            // Describe and create a render target view (RTV) descriptor heap.
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
            rtvHeapDesc.NumDescriptors = FrameCount;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap)), "Failed to create RTV heap.");

            m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // Create frame resources.
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());

            // Create an RTV for each frame.
            for (UINT i = 0; i < FrameCount; i++) {
                ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])), "Failed to get RTV buffer.");
                m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
                rtvHandle.Offset(1, m_RtvDescriptorSize);
            }
        }

        ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)), "Failed to create command allocator.");
    }

    void Application::LoadAssets() {
        // Create the command list.
        ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));
        
        // Command lists are created in the recording state, but there is nothing
        // to record yet. The main loop expects it to be closed, so close it now.
        ThrowIfFailed(m_CommandList->Close());

        // Create synchronization objects.
        {
            ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
            m_FenceValue = 1;

            // Create an event handle to use for frame synchronization.
            m_FrameEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (m_FrameEvent == nullptr)
            {
                ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
            }
        }
    }


    void Application::PopulateCommandList() const {
        // Command list allocators can only be reset when the associated 
        // command lists have finished execution on the GPU; apps should use 
        // fences to determine GPU execution progress.
        ThrowIfFailed(m_CommandAllocator->Reset());

        // However, when ExecuteCommandList() is called on a particular command 
        // list, that command list can then be reset at any time and must be before 
        // re-recording.
        ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineState.Get()));

        // Indicate that the back buffer will be used as a render target.
        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
            m_RenderTargets[m_FrameIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );
        m_CommandList->ResourceBarrier(1, &transition);

        const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex,
                                                      m_RtvDescriptorSize);

        // Record commands.
        constexpr float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
        m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        // Indicate that the back buffer will now be used to present.
        transition = CD3DX12_RESOURCE_BARRIER::Transition(
            m_RenderTargets[m_FrameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        );
        m_CommandList->ResourceBarrier(1, &transition);

        ThrowIfFailed(m_CommandList->Close());
    }

    void Application::WaitForPreviousFrame() {
        // Signal and increment the fence value.
        const UINT64 fence = m_FenceValue;
        ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fence));
        m_FenceValue++;

        // Wait until the previous frame is finished.
        if (m_Fence->GetCompletedValue() < fence) {
            ThrowIfFailed(m_Fence->SetEventOnCompletion(fence, m_FrameEvent));
            WaitForSingleObject(m_FrameEvent, INFINITE);
        }

        m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
    }

    _Use_decl_annotations_
    void Application::GetHardwareAdapter(IDXGIFactory1* pFactory,
                                         IDXGIAdapter1** ppAdapter,
                                         const bool requestHighPerformanceAdapter) {
        *ppAdapter = nullptr;

        ComPtr<IDXGIAdapter1> adapter;

        ComPtr<IDXGIFactory6> factory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
            for (
                UINT adapterIndex = 0;
                SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                    adapterIndex,
                    requestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE :
                                                    DXGI_GPU_PREFERENCE_UNSPECIFIED,
                    IID_PPV_ARGS(&adapter)));
                ++adapterIndex) {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }
                
                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                    break;
                }
            }
        }

        if (adapter.Get() == nullptr) {
            for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex) {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }
                
                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                    break;
                }
            }
        }

        *ppAdapter = adapter.Detach();
    }

    std::wstring Application::GetAssetFullPath(const std::wstring& assetName) {
        return L"Resources/" + assetName;
    }

    void Application::OnWindowSizeChanged(const int width, const int height) {
        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }
}
