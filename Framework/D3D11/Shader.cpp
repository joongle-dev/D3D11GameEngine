#include "Framework.h"
#include "Shader.h"
#include <iostream>

Shader::Shader(Context * context)
{
	m_graphics = context->GetSubsystem<Graphics>();
}

void Shader::Create(const std::string& path, const std::string& vs, const std::string& ps, const D3D_SHADER_MACRO* macro)
{
	HRESULT hr;
	LPD3D10BLOB error = nullptr;

	std::wstring filename = std::wstring(path.begin(), path.end());

	hr = D3DCompileFromFile(
		filename.c_str(),
		macro,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		ps.c_str(),
		"ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS,
		NULL,
		mBytecode.ReleaseAndGetAddressOf(),
		&error);
	assert(ShaderErrorHandler(hr, error));

	hr = m_graphics->GetDevice()->CreatePixelShader(
		mBytecode->GetBufferPointer(),
		mBytecode->GetBufferSize(),
		NULL,
		mPixelShader.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	hr = D3DCompileFromFile(
		filename.c_str(),
		macro,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		vs.c_str(), 
		"vs_5_0", 
		D3DCOMPILE_ENABLE_STRICTNESS, 
		NULL, 
		mBytecode.ReleaseAndGetAddressOf(), 
		&error);
	assert(ShaderErrorHandler(hr, error));

	hr = m_graphics->GetDevice()->CreateVertexShader(
		mBytecode->GetBufferPointer(), 
		mBytecode->GetBufferSize(), 
		NULL, 
		mVertexShader.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
}

void Shader::Bind()
{
	m_graphics->GetDeviceContext()->VSSetShader(mVertexShader.Get(), nullptr, 0);
	m_graphics->GetDeviceContext()->PSSetShader(mPixelShader.Get(), nullptr, 0);
}

const bool Shader::ShaderErrorHandler(HRESULT hr, LPD3D10BLOB error)
{
	if (FAILED(hr)) {
		if (error) {
			char* compileErrors;
			compileErrors = (char*)(error->GetBufferPointer());

			for (size_t i = 0; i < error->GetBufferSize(); i++)
				std::cout << compileErrors[i];
			printf("\nFailed to compile shader.");

			error->Release();
			delete error;
		}
		return false;
	}
	return true;
}
