#pragma once
#include "Subsystem.h"

struct CameraBuffer
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
	DirectX::XMVECTOR campos;
};

struct WorldBuffer
{
	DirectX::XMMATRIX world;
};

struct LightBuffer
{
	DirectX::XMVECTOR lightpos;
	DirectX::XMVECTOR lightdir;
};

struct GizmoVertex
{
	Vector3 position;
	Vector4 color;
};

enum ShaderFlags : unsigned int
{
	DEPTH_PASS        = 1 << 0,
	DIRECTIONAL_LIGHT = 1 << 1,
	POINT_LIGHT       = 1 << 2,
	SPOT_LIGHT        = 1 << 3,
	ALBEDO_TEXTURE    = 1 << 4,
	ROUGHNESS_TEXTURE = 1 << 5,
	METALLIC_TEXTURE  = 1 << 6,
	EMISSIVE_TEXTURE  = 1 << 7,
	NORMAL_TEXTURE    = 1 << 8,
	HEIGHT_TEXTURE    = 1 << 9,
	SPECULAR_TEXTURE  = 1 << 10,
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

	class Shader* GetMatchingShader(unsigned int flags);

	class RenderTarget* GetMainTarget() const { return mMainTarget; }

private:
	class Graphics* mGraphics;

	class RenderTarget* mMainTarget;

	class ConstantBuffer<CameraBuffer>* mCameraBuffer;
	class ConstantBuffer<WorldBuffer>* mWorldBuffer;
	class ConstantBuffer<LightBuffer>* mLightBuffer;
	class InputLayout* mInputLayout;


	class DepthStencilState* mLightPassDepth;
	class BlendState* mDepthPassBlend;
	class BlendState* mLightPassBlend;

	std::map<unsigned int, std::unique_ptr<class Shader>> mShaders;
};