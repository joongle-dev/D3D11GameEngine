#pragma once

class InputLayout final
{
public:
	InputLayout(class Context* context);
	~InputLayout() = default;

	InputLayout(const InputLayout&) = delete;
	InputLayout& operator=(const InputLayout&) = delete;

	void Create(LPD3D10BLOB bytecode, D3D11_INPUT_ELEMENT_DESC* elements, const unsigned int count);
	void Create(LPD3D10BLOB bytecode);

	void BindPipeline();

private:
	class Graphics* m_graphics;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;
};