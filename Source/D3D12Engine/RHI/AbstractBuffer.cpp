// Copyright (C) 2025 Jean "Pixfri" Letessier 
// This file is part of D3D12Engine.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include <D3D12Engine/RHI/AbstractBuffer.hpp>

#include <D3D12Engine/RHI/DxUtils.hpp>

#include <format>

namespace D3D12Engine {
    AbstractBuffer::AbstractBuffer(ID3D12Device* pDevice, const size_t size)
        : m_Device(pDevice) {
        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto ibResDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        ThrowIfFailed(m_Device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &ibResDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_Buffer))
        );
    }

    void AbstractBuffer::Map(UINT8** pDataBegin) const {
        const CD3DX12_RANGE readRange(0, 0);
        ThrowIfFailed(m_Buffer->Map(0, &readRange, reinterpret_cast<void**>(pDataBegin)));
    }

    void AbstractBuffer::Unmap() const {
        m_Buffer->Unmap(0, nullptr);
    }
}
