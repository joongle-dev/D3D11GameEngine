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

struct LightBuffer
{
	Vector3 lightpos;
	Vector3 lightdir;
	float padding[2];
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

	void RenderCamera(class Scene* scene, class Camera* camera);

	class Shader* GetMatchingShader(unsigned short flags);

	class RenderTarget* GetMainTarget() const { return m_mainTarget; }

private:
	class Graphics* m_graphics;

	class RenderTarget* m_mainTarget;

	class ConstantBuffer<CameraBuffer>* m_CameraBuffer;
	class ConstantBuffer<WorldBuffer>* m_WorldBuffer;
	class ConstantBuffer<LightBuffer>* m_LightBuffer;
	class InputLayout* m_layout;

	class Camera* m_camera;

	std::map<unsigned short, class Shader*> m_shaders;

	class Shader* gizmoshader;
	class InputLayout* gizmolayout;
	class VertexBuffer* gizmovertices;
};