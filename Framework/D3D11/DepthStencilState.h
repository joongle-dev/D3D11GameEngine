#pragma once

class DepthStencilState final
{
public:
	DepthStencilState(class Context* context);
	~DepthStencilState() = default;

	void Create();
	void Bind();

	D3D11_DEPTH_STENCIL_DESC* Map() { return &mDepthStencilDesc; }

private:
	class Graphics* mGraphics;

	D3D11_DEPTH_STENCIL_DESC mDepthStencilDesc;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
};

