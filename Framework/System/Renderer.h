#pragma once
#include "Subsystem.h"

struct CameraBuffer
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
	DirectX::XMFLOAT3 campos;
	float padding;
};

struct WorldBuffer
{
	DirectX::XMMATRIX world;
};

class Renderer final : public Subsystem<Renderer>
{
public:
	Renderer(class Context* context);
	~Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void Update() override;

	class RenderTarget* GetMainTarget() const { return m_mainTarget; }

	class Shader* GetMatchingShader(unsigned short flags);

private:
	class Graphics* m_graphics;

	class RenderTarget* m_mainTarget;

	class ConstantBuffer<CameraBuffer>* m_cameraBuffer;
	class ConstantBuffer<WorldBuffer>* m_worldBuffer;
	class InputLayout* m_layout;
	class Shader* m_shader;

	class Camera* m_camera;

	std::map<unsigned short, std::unique_ptr<class Shader>> m_shaders;
};