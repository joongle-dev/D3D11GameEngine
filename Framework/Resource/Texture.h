#pragma once
#include "Resource.h"

class Texture final : public Resource<Texture>
{
public:
	Texture(class Context* context);
	~Texture() = default;

	void LoadFromFile(const std::string& filename) override;
	void SaveToFile(const std::string& filename) override;

	ID3D11ShaderResourceView* GetShaderResource() const { return m_shaderResourceView.Get(); }

private:
	unsigned int m_width;
	unsigned int m_height;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

	class Graphics* m_graphics;
};