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

struct GizmoVertex
{
	Vector3 position;
	Vector4 color;
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

	void RenderCamera(class Scene* scene, class Camera* camera);

private:
	class Graphics* m_graphics;

	class RenderTarget* m_mainTarget;

	class ConstantBuffer<CameraBuffer>* m_cameraBuffer;
	class ConstantBuffer<WorldBuffer>* m_worldBuffer;
	class InputLayout* m_layout;

	class Camera* m_camera;

	std::map<unsigned short, class Shader*> m_shaders;

	class Shader* gizmoshader;
	class InputLayout* gizmolayout;
	class VertexBuffer* gizmovertices;
};