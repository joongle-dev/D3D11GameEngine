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

	class Shader* GetMatchingShader(unsigned char flags);

	class RenderTarget* GetMainTarget() const { return mMainTarget; }

private:
	class Graphics* mGraphics;

	class RenderTarget* mMainTarget;

	class ConstantBuffer<CameraBuffer>* mCameraBuffer;
	class ConstantBuffer<WorldBuffer>* mWorldBuffer;
	class ConstantBuffer<LightBuffer>* mLightBuffer;
	class InputLayout* mInputLayout;

	std::map<unsigned char, std::unique_ptr<class Shader>> mShaders;
};