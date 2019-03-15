#pragma once

class BlendState final
{
public:
	BlendState(class Context* context);
	~BlendState() = default;

	void Create();
	void BindPipeline();

private:
	class Graphics* m_graphics;
	D3D11_BLEND_DESC m_desc;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_state;
};

