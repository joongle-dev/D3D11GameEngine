#include "Framework.h"
#include "Texture.h"
#include <PlatformHelpers.h>
#include <WICTextureLoader.h>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

using namespace DirectX;

Texture::Texture(Context * context) :
	Resource<Texture>(context)
{
	m_graphics = context->GetSubsystem<Graphics>();
}

void Texture::LoadFromFile(const std::string & filename)
{
	//Set name as the filename
	size_t firstindex = mName.find_last_of('/');
	if (firstindex >= mName.length()) firstindex = 0;
	size_t lastindex = mName.find_last_of('.');
	mName = mName.substr(firstindex, lastindex);

	HRESULT hr = CreateWICTextureFromFile(
		m_graphics->GetDevice(), 
		std::wstring(filename.begin(), filename.end()).c_str(), 
		nullptr, 
		m_shaderResourceView.ReleaseAndGetAddressOf());

	ThrowIfFailed(hr);
}

void Texture::SaveToFile(const std::string & filename)
{
}
