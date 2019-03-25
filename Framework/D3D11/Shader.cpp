#include "Framework.h"
#include "Shader.h"
#include <iostream>

Shader::Shader(Context * context)
{
	m_graphics = context->GetSubsystem<Graphics>();
}

void Shader::Create(std::string path, std::string vs, std::string ps)
{
	HRESULT hr;
	LPD3D10BLOB error = nullptr;

	std::wstring filename = std::wstring(path.begin(), path.end());

	hr = D3DCompileFromFile(
		filename.c_str(),
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		ps.c_str(),
		"ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS,
		NULL,
		m_bytecode.ReleaseAndGetAddressOf(),
		&error);
	assert(ShaderErrorHandler(hr, error));

	hr = m_graphics->GetDevice()->CreatePixelShader(
		m_bytecode->GetBufferPointer(),
		m_bytecode->GetBufferSize(),
		NULL,
		m_pixel.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	hr = D3DCompileFromFile(
		filename.c_str(),
		NULL, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		vs.c_str(), 
		"vs_5_0", 
		D3DCOMPILE_ENABLE_STRICTNESS, 
		NULL, 
		m_bytecode.ReleaseAndGetAddressOf(), 
		&error);
	assert(ShaderErrorHandler(hr, error));

	hr = m_graphics->GetDevice()->CreateVertexShader(
		m_bytecode->GetBufferPointer(), 
		m_bytecode->GetBufferSize(), 
		NULL, 
		m_vertex.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
}

void Shader::Bind()
{
	m_graphics->GetDeviceContext()->VSSetShader(m_vertex.Get(), nullptr, 0);
	m_graphics->GetDeviceContext()->PSSetShader(m_pixel.Get(), nullptr, 0);
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
