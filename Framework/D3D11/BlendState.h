#pragma once

class BlendState final
{
public:
	BlendState(class Context* context);
	~BlendState() = default;

	void Create();
	void Bind();

	D3D11_BLEND_DESC* Map() { return &mBlendDesc; }

private:
	class Graphics* mGraphics;

	D3D11_BLEND_DESC mBlendDesc;
	Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState;
};

