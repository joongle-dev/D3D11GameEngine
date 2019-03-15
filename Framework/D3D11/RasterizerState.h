#pragma once

class RasterizerState final
{
public:
	RasterizerState(class Context* context);
	~RasterizerState() = default;

	void Create();
	void Bind();

private:
	class Graphics* m_graphics;

	D3D11_RASTERIZER_DESC m_desc;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_state;
};