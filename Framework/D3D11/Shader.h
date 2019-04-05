#pragma once

class Shader final
{
public:
	Shader(class Context* context);
	~Shader() = default;

	void Create(const std::string& path, const std::string& vs = "VS", const std::string& ps = "PS", const D3D_SHADER_MACRO* macro = nullptr);
	void Bind();

	const bool ShaderErrorHandler(HRESULT hr, LPD3D10BLOB error);

	ID3D10Blob* GetBytecode() const { return mBytecode.Get(); }

private:
	class Graphics* m_graphics;

	Microsoft::WRL::ComPtr<ID3D10Blob> mBytecode;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
};