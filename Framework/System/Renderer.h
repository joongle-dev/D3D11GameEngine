#pragma once
#include "Subsystem.h"

#define NUM_DIRECTIONALLIGHTS 4
#define NUM_POINTLIGHTS 32
#define NUM_SPOTLIGHTS 32

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

struct BoneBuffer
{
	DirectX::XMMATRIX bones[96];
};

struct LightBuffer
{
	unsigned int numDirectionalLight;
	unsigned int numPointLight;
	unsigned int numSpotLight;
	unsigned int padding[1];

	struct
	{
		Vector3 color;
		float padding1;
		Vector3 direction;
		float padding2;
	} directionalLight[4];

	struct
	{
		Vector3 color;
		float range;
		Vector3 position;
		float intensity;
	} pointLight[32];

	struct
	{
		Vector3 color;
		float range;
		Vector3 position;
		float intensity;
		Vector3 direction;
		float spotangle;
	} spotLight[32];
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
	SKINNED			  = 1 << 11,
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
	class ConstantBuffer<BoneBuffer>* mBoneBuffer;
	class ConstantBuffer<LightBuffer>* mLightBuffer;
	class InputLayout* mInputLayout;


	class DepthStencilState* mLightPassDepth;
	class BlendState* mDepthPassBlend;
	class BlendState* mLightPassBlend;

	std::map<unsigned int, std::unique_ptr<class Shader>> mShaders;
};