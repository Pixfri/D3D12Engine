// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <format>

#include <D3D12Engine/RHI/DxUtils.hpp>

namespace D3D12Engine {
    template <typename T>
    VertexBuffer<T>::VertexBuffer(ID3D12Device* device, const T* data, size_t size)
        : AbstractBuffer(device, size) {
        auto name = std::format("Vertex buffer of size {}B and type {}", size, typeid(T).name());
        const std::wstring wName(name.begin(), name.end());
        ThrowIfFailed(m_Buffer->SetName(wName.c_str()));
        UINT8* pVertexDataBegin;

        Map(&pVertexDataBegin);
        memcpy(pVertexDataBegin, data, size);
        Unmap();

        m_BufferView.BufferLocation = m_Buffer->GetGPUVirtualAddress();
        m_BufferView.StrideInBytes = sizeof(T);
        m_BufferView.SizeInBytes = size;
    }

    template <typename T>
    void VertexBuffer<T>::Apply(ID3D12GraphicsCommandList* commandList) const {
        commandList->IASetVertexBuffers(0, 1, &m_BufferView);
    }

}
