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
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		switch (usage)
		{
			case D3D11_USAGE_DEFAULT:
				desc.CPUAccessFlags = 0;
				break;
			case D3D11_USAGE_DYNAMIC:
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				break;
		}
		desc.Usage = usage;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(T);

		auto hr = m_graphics->GetDevice()->CreateBuffer
		(
			&desc,
			nullptr,
			m_buffer.ReleaseAndGetAddressOf()
		);
		assert(SUCCEEDED(hr));
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

	void BindPipeline(const ShaderType type, const unsigned int slot)
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