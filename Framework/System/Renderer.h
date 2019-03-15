#pragma once
#include "Subsystem.h"

struct CameraBuffer
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
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

	void SetCamera(class Camera* camera) { m_camera = camera; }

private:
	class Graphics* m_graphics;
	class IComponentContainer* m_meshRenderers;

	class ConstantBuffer<CameraBuffer>* m_cameraBuffer;
	class ConstantBuffer<WorldBuffer>* m_worldBuffer;
	class InputLayout* m_layout;
	class Shader* m_shader;

	class Camera* m_camera;
};