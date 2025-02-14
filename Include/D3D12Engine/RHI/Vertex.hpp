// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#ifndef DE_RHI_VERTEX_HPP
#define DE_RHI_VERTEX_HPP

#include <D3D12Engine/pch.hpp>

namespace D3D12Engine {
    struct VertexPosColor {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT4 Color;
    };
}

#endif // DE_RHI_VERTEX_HPP
