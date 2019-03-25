#pragma once

class RenderTarget
{
public:
	RenderTarget(class Context* context);
	~RenderTarget() = default;

	void Create(
		const UINT& width, 
		const UINT& height,
		const DXGI_FORMAT& pixel = DXGI_FORMAT_R8G8B8A8_UNORM,
		const DXGI_FORMAT& depth = DXGI_FORMAT_D24_UNORM_S8_UINT
	);
	void Bind();
	void Clear(const Color& color);

	ID3D11ShaderResourceView* GetShaderResource() const { return m_shaderresource.Get(); }

	const UINT GetWidth() const { return static_cast<UINT>(m_viewport.Width); }
	const UINT GetHeight() const { return static_cast<UINT>(m_viewport.Height); }

private:
	class Graphics* m_graphics;

	D3D11_VIEWPORT m_viewport;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_depthstencil;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_rendertarget;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderresource;
};