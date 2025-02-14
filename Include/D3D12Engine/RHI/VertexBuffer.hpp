// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#ifndef DE_RHI_VERTEXBUFFER_HPP
#define DE_RHI_VERTEXBUFFER_HPP

#include <D3D12Engine/pch.hpp>

#include <D3D12Engine/RHI/AbstractBuffer.hpp>

namespace D3D12Engine {
    template <typename T>
    class VertexBuffer final : public AbstractBuffer {
    public:
        VertexBuffer(ID3D12Device* device, const T* data, size_t size);
        void Apply(ID3D12GraphicsCommandList* commandList) const;

    private:
        D3D12_VERTEX_BUFFER_VIEW m_BufferView;
    };
}

#include <D3D12Engine/RHI/VertexBuffer.inl>

#endif // DE_RHI_VERTEXBUFFER_HPP
