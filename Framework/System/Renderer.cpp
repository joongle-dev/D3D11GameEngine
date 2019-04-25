#include "Framework.h"
#include "Renderer.h"

using namespace DirectX;

Renderer::Renderer(Context * context) :
	Subsystem<Renderer>(context)
{
	mGraphics = context->GetSubsystem<Graphics>();

	mMainTarget = new RenderTarget(context);
	mMainTarget->Create(1920, 1080);

	mCameraBuffer = new ConstantBuffer<CameraBuffer>(context);
	mWorldBuffer = new ConstantBuffer<WorldBuffer>(context);
	mBoneBuffer = new ConstantBuffer<BoneBuffer>(context);
	mLightBuffer = new ConstantBuffer<LightBuffer>(context);

	D3D11_INPUT_ELEMENT_DESC Desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mInputLayout = new InputLayout(context);
	mInputLayout->Create(GetMatchingShader(0)->GetBytecode(), Desc, sizeof(Desc) / sizeof(Desc[0]));

	RasterizerState* raster = new RasterizerState(context);
	raster->Create();
	raster->Bind();
}

void Renderer::Update()
{
	auto pCurrentScene = m_context->GetSubsystem<SceneManager>()->GetCurrentScene();

	mInputLayout->Bind();

	auto pLightData = mLightBuffer->Map();
	ZeroMemory(pLightData, sizeof(LightBuffer));
	for (auto itLight = pCurrentScene->ComponentBegin<Light>(); itLight != pCurrentScene->ComponentEnd<Light>(); itLight++)
	{
		Transform* pLightTransform = itLight->GetTransform();
		switch (itLight->GetLightType())
		{
			case Light::Directional:
			{
				auto& light = pLightData->directionalLight[pLightData->numDirectionalLight++];

				light.color     = itLight->GetColor();
				light.direction = pLightTransform->GetForward();
				break;
			}
			case Light::Point:
			{
				auto& light = pLightData->pointLight[pLightData->numPointLight++];

				light.color     = itLight->GetColor();
				light.range     = itLight->GetRange();
				light.intensity = itLight->GetIntensity();
				light.position  = pLightTransform->GetPosition();
				break;
			}
			case Light::Spot:
			{
				auto& light = pLightData->spotLight[pLightData->numSpotLight++];

				light.color     = itLight->GetColor();
				light.range     = itLight->GetRange();
				light.intensity = itLight->GetIntensity();
				light.spotangle = itLight->GetSpotAngle();
				light.position  = pLightTransform->GetPosition();
				light.direction = pLightTransform->GetForward();
				break;
			}
		}
	}
	mLightBuffer->Unmap();
	mLightBuffer->Bind(ShaderType::PS, 0);

	for (auto itCamera = pCurrentScene->ComponentBegin<Camera>(); itCamera != pCurrentScene->ComponentEnd<Camera>(); itCamera++)
		RenderCamera(pCurrentScene, itCamera);
}

void Renderer::RenderCamera(Scene* scene, Camera * camera)
{
	RenderTarget* pRenderTarget = camera->GetRenderTarget();

	if (!pRenderTarget)
		return;

	pRenderTarget->Clear(Color(0.5f, 0.5f, 0.5f, 1.0f));
	pRenderTarget->Bind();

	Transform* pCameraTransform = camera->GetTransform();
	auto pCameraData = mCameraBuffer->Map();
	{
		pCameraData->view = XMMatrixTranspose(camera->GetViewMatrix());
		pCameraData->projection = XMMatrixTranspose(camera->GetProjectionMatrix());
		pCameraData->campos = camera->GetTransform()->GetPosition();
	}
	mCameraBuffer->Unmap();
	mCameraBuffer->Bind(ShaderType::VS, 0);

	mGraphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride[] = { sizeof(Vector3), sizeof(Vector3), sizeof(Vector3), sizeof(Vector2), sizeof(Vector4), sizeof(Vector4) };
	UINT offset[] = { 0, 0, 0, 0, 0, 0 };

	for (auto itRenderable = scene->ComponentBegin<MeshRenderer>(); itRenderable != scene->ComponentEnd<MeshRenderer>(); itRenderable++)
	{
		Transform* pRenderableTransform = itRenderable->GetTransform();
		auto pWorldData = mWorldBuffer->Map();
		{
			pWorldData->world = XMMatrixTranspose(pRenderableTransform->GetWorldTransform());
		}
		mWorldBuffer->Unmap();
		mWorldBuffer->Bind(ShaderType::VS, 1);
	
		Mesh* pMesh = itRenderable->GetMesh();
		if (!pMesh) continue;
	
		Material* pMaterial = itRenderable->GetMaterial();
		if (!pMaterial) continue;
	
		GetMatchingShader(pMaterial->GetShaderFlags())->Bind();

		ID3D11Buffer* vbs[] =
		{
			pMesh->GetPositionBuffer(),
			pMesh->GetNormalBuffer(),
			pMesh->GetTangentBuffer(),
			pMesh->GetTexcoordBuffer(),
		};
	
		ID3D11ShaderResourceView* srv[] =
		{
			pMaterial->GetShaderResource(Material::Albedo),
			pMaterial->GetShaderResource(Material::Roughness),
			pMaterial->GetShaderResource(Material::Metallic),
			pMaterial->GetShaderResource(Material::Normal),
			pMaterial->GetShaderResource(Material::Height),
		};
	
		mGraphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mGraphics->GetDeviceContext()->IASetVertexBuffers(0, 4, vbs, stride, offset);
		mGraphics->GetDeviceContext()->IASetIndexBuffer(pMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		mGraphics->GetDeviceContext()->PSSetShaderResources(0, 5, srv);

		mGraphics->GetDeviceContext()->DrawIndexed(pMesh->GetIndexCount(), 0, 0);
	}

	for (auto itRenderable = scene->ComponentBegin<SkinnedMeshRenderer>(); itRenderable != scene->ComponentEnd<SkinnedMeshRenderer>(); itRenderable++)
	{
		//Transform* pRenderableTransform = itRenderable->GetTransform();
		auto pBoneData = mBoneBuffer->Map();
		{

		}
		mBoneBuffer->Unmap();
		mBoneBuffer->Bind(ShaderType::VS, 1);

		Mesh* pMesh = itRenderable->GetMesh();
		if (!pMesh) continue;

		Material* pMaterial = itRenderable->GetMaterial();
		if (!pMaterial) continue;

		GetMatchingShader(pMaterial->GetShaderFlags() | ShaderFlags::SKINNED)->Bind();

		ID3D11Buffer* vbs[] =
		{
			pMesh->GetPositionBuffer(),
			pMesh->GetNormalBuffer(),
			pMesh->GetTangentBuffer(),
			pMesh->GetTexcoordBuffer(),
			pMesh->GetBlendIndicesBuffer(),
			pMesh->GetBlendWeightsBuffer(),
		};

		ID3D11ShaderResourceView* srv[] =
		{
			pMaterial->GetShaderResource(Material::Albedo),
			pMaterial->GetShaderResource(Material::Roughness),
			pMaterial->GetShaderResource(Material::Metallic),
			pMaterial->GetShaderResource(Material::Normal),
			pMaterial->GetShaderResource(Material::Height),
		};

		mGraphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mGraphics->GetDeviceContext()->IASetVertexBuffers(0, 6, vbs, stride, offset);
		mGraphics->GetDeviceContext()->IASetIndexBuffer(pMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		mGraphics->GetDeviceContext()->PSSetShaderResources(0, 5, srv);

		mGraphics->GetDeviceContext()->DrawIndexed(pMesh->GetIndexCount(), 0, 0);
	}
}

Shader * Renderer::GetMatchingShader(unsigned int flags)
{
	auto iter = mShaders.emplace(flags, std::make_unique<Shader>(m_context));
	if (iter.second)
	{
		D3D_SHADER_MACRO macros[] = {
			{ "ALBEDO_TEXTURE",    flags & ALBEDO_TEXTURE    ? "1" : "0" },
			{ "ROUGHNESS_TEXTURE", flags & ROUGHNESS_TEXTURE ? "1" : "0" },
			{ "METALLIC_TEXTURE",  flags & METALLIC_TEXTURE  ? "1" : "0" },
			{ "NORMAL_TEXTURE",    flags & NORMAL_TEXTURE    ? "1" : "0" },
			{ "HEIGHT_TEXTURE",    flags & HEIGHT_TEXTURE    ? "1" : "0" },
			{ "SKINNED",           flags & SKINNED           ? "1" : "0" },
			{ nullptr, nullptr }
		};
		iter.first->second->Create("../Assets/Shader/Default1.hlsl", "VS", "PS", macros);
	}
	return iter.first->second.get();
}
