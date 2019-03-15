#pragma once

class DepthStencilState final
{
public:
	DepthStencilState(class Context* context);
	~DepthStencilState() = default;

	void Create();
	void BindPipeline();

private:
	class Graphics* m_graphics;

	D3D11_DEPTH_STENCIL_DESC m_desc;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_state;
};

