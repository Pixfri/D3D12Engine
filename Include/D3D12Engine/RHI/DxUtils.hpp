// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#ifndef DE_DXUTILS_HPP
#define DE_DXUTILS_HPP

#include <D3D12Engine/pch.hpp>

#include <stdexcept>

namespace D3D12Engine {
    inline void ThrowIfFailed(const HRESULT hr, const char* msg = "") {
        if (FAILED(hr)) {
            throw std::exception(msg);
        }
    }
}

#endif // DE_DXUTILS_HPP
