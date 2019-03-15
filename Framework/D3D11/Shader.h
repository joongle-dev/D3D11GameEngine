#pragma once

class Shader final
{
public:
	Shader(class Context* context);
	~Shader() = default;

	void Create(std::string path, std::string vs = "VS", std::string ps = "PS");
	void Bind();

	const bool ShaderErrorHandler(HRESULT hr, LPD3D10BLOB error);

	ID3D10Blob* GetBytecode() const { return m_bytecode.Get(); }

private:
	class Graphics* m_graphics;

	Microsoft::WRL::ComPtr<ID3D10Blob> m_bytecode;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertex;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel;
};