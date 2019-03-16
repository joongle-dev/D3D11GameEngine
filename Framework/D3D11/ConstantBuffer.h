#pragma once
#include "Framework.h"

enum class ShaderType : unsigned int
{
	VS, HS, DS, GS, PS, CS
};

template <typename T>
class ConstantBuffer final
{
public:
	ConstantBuffer(class Context* context, const D3D11_USAGE& usage = D3D11_USAGE_DYNAMIC)
	{
		m_graphics = context->GetSubsystem<class Graphics>();

		m_graphics->CreateBuffer(
			m_buffer.ReleaseAndGetAddressOf(),
			D3D11_BIND_CONSTANT_BUFFER,
			sizeof(T),
			nullptr,
			usage);
	}
	~ConstantBuffer() = default;

	T* Map()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		auto hr = m_graphics->GetDeviceContext()->Map
		(
			m_buffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource
		);
		assert(SUCCEEDED(hr));

		return static_cast<T*>(mappedResource.pData);
	}
	void Unmap()
	{
		m_graphics->GetDeviceContext()->Unmap(m_buffer.Get(), 0);
	}

	void Bind(const ShaderType type, const unsigned int slot)
	{
		switch (type)
		{
			case ShaderType::VS: m_graphics->GetDeviceContext()->VSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf()); break;
			case ShaderType::PS: m_graphics->GetDeviceContext()->PSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf()); break;
			case ShaderType::HS: m_graphics->GetDeviceContext()->HSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf()); break;
			case ShaderType::DS: m_graphics->GetDeviceContext()->DSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf()); break;
			case ShaderType::GS: m_graphics->GetDeviceContext()->GSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf()); break;
			case ShaderType::CS: m_graphics->GetDeviceContext()->CSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf()); break;
		}
	}

private:
	class Graphics* m_graphics;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};