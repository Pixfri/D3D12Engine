// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#ifndef DE_BUFFER_HPP
#define DE_BUFFER_HPP

#include <D3D12Engine/pch.hpp>

namespace D3D12Engine {
    class AbstractBuffer {
    public:
        AbstractBuffer(ID3D12Device* pDevice, size_t size);
        ~AbstractBuffer() = default;

        [[nodiscard]] inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

        void Map(UINT8** pDataBegin) const;
        void Unmap() const;
    
    protected:
        ComPtr<ID3D12Resource> m_Buffer;
        ID3D12Device* m_Device{nullptr};
    };
}

#include <D3D12Engine/RHI/AbstractBuffer.inl>

#endif // DE_BUFFER_HPP
