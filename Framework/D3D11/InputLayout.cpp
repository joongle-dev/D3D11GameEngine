#include "Framework.h"
#include "InputLayout.h"

InputLayout::InputLayout(Context * context)
{
	m_graphics = context->GetSubsystem<Graphics>();
}

void InputLayout::Create(LPD3D10BLOB bytecode, D3D11_INPUT_ELEMENT_DESC * elements, const unsigned int count)
{
	if (elements == nullptr || count == 0)
		assert(false);

	auto hr = m_graphics->GetDevice()->CreateInputLayout
	(
		elements,
		count,
		bytecode->GetBufferPointer(),
		bytecode->GetBufferSize(),
		m_layout.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(hr));
}

void InputLayout::Create(LPD3D10BLOB bytecode)
{
	if (bytecode == nullptr)
		assert(false);

	//Create Shader Reflection
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflector = nullptr;
	auto hr = D3DReflect
	(
		bytecode->GetBufferPointer(),
		bytecode->GetBufferSize(),
		__uuidof(ID3D11ShaderReflection), // IID_ID3D11ShaderReflection
		reinterpret_cast<void**>(reflector.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(hr));

	//Get Shader Information
	D3D11_SHADER_DESC shaderDesc;
	reflector->GetDesc(&shaderDesc);

	//Get Element Information
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs;
	for (unsigned int i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflector->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		//Check DXGI Format
		std::string temp = elementDesc.SemanticName;
		if (temp == "POSITION")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		else if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		inputLayoutDescs.emplace_back(elementDesc);
	}

	hr = m_graphics->GetDevice()->CreateInputLayout
	(
		inputLayoutDescs.data(),
		inputLayoutDescs.size(),
		bytecode->GetBufferPointer(),
		bytecode->GetBufferSize(),
		m_layout.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(hr));
}

void InputLayout::Bind()
{
	m_graphics->GetDeviceContext()->IASetInputLayout(m_layout.Get());
}
