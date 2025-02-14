// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#ifndef DE_PCH_HPP
#define DE_PCH_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <directx/d3dx12.h>

#include <string>
#include <wrl/client.h>
#include <shellapi.h>

namespace D3D12Engine {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
}

#endif // DE_PCH_HPP