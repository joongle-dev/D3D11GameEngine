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
	mLightBuffer = new ConstantBuffer<LightBuffer>(context);

	D3D11_INPUT_ELEMENT_DESC Desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mInputLayout = new InputLayout(context);
	mInputLayout->Create(GetMatchingShader(0)->GetBytecode(), Desc, sizeof(Desc) / sizeof(Desc[0]));

	mLightPassDepth = new DepthStencilState(context);
	mLightPassDepth->Map()->DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	mLightPassDepth->Create();
	mLightPassDepth->Bind();

	mDepthPassBlend = new BlendState(context);
	mDepthPassBlend->Create();

	mLightPassBlend = new BlendState(context);
	D3D11_BLEND_DESC* blendstate = mLightPassBlend->Map();
	blendstate->RenderTarget[0].BlendEnable = true;
	blendstate->RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendstate->RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendstate->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendstate->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendstate->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendstate->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	mLightPassBlend->Create();
	mLightPassBlend->Bind();

	RasterizerState* raster = new RasterizerState(context);
	raster->Create();
	raster->Bind();
}

void Renderer::Update()
{
	auto pCurrentScene = m_context->GetSubsystem<SceneManager>()->GetCurrentScene();

	mInputLayout->Bind();

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

	UINT stride[] = { sizeof(Vector3), sizeof(Vector3), sizeof(Vector3), sizeof(Vector2) };
	UINT offset[] = { 0, 0, 0, 0 };

	mInputLayout->Bind();
	mGraphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

		ID3D11Buffer* vbs[] =
		{
			pMesh->GetPositions(),
		};

		mGraphics->GetDeviceContext()->IASetVertexBuffers(0, 1, vbs, stride, offset);
		mGraphics->GetDeviceContext()->IASetIndexBuffer(pMesh->GetIndices(), DXGI_FORMAT_R32_UINT, 0);

		GetMatchingShader(DEPTH_PASS)->Bind();
		mDepthPassBlend->Bind();
		mGraphics->GetDeviceContext()->DrawIndexed(pMesh->GetIndexCount(), 0, 0);
	}

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
		
		ID3D11Buffer* vbs[] =
		{
			pMesh->GetPositions(),
			pMesh->GetNormals(),
			pMesh->GetTangents(),
			pMesh->GetTexcoords(),
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
		mGraphics->GetDeviceContext()->IASetIndexBuffer(pMesh->GetIndices(), DXGI_FORMAT_R32_UINT, 0);
		mGraphics->GetDeviceContext()->PSSetShaderResources(0, 5, srv);
		
		for (auto itLight = scene->ComponentBegin<Light>(); itLight != scene->ComponentEnd<Light>(); itLight++)
		{
			Transform* pLightTransform = itLight->GetTransform();
			auto pLightData = mLightBuffer->Map();
			{
				pLightData->lightpos = pLightTransform->GetPosition();
				pLightData->lightdir = pLightTransform->GetForward();
			}
			mLightBuffer->Unmap();
			mLightBuffer->Bind(ShaderType::VS, 2);
		
			switch (itLight->GetLightType())
			{
				case Light::Directional:
					GetMatchingShader(pMaterial->GetShaderFlags() | DIRECTIONAL_LIGHT)->Bind();
					break;
				case Light::Point:
					GetMatchingShader(pMaterial->GetShaderFlags() | POINT_LIGHT)->Bind();
					break;
			}
			mLightPassBlend->Bind();
			mGraphics->GetDeviceContext()->DrawIndexed(pMesh->GetIndexCount(), 0, 0);
		}
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
			{ "DEPTH_PASS",        flags & DEPTH_PASS        ? "1" : "0" },
			{ "DIRECTIONAL_LIGHT", flags & DIRECTIONAL_LIGHT ? "1" : "0" },
			{ "POINT_LIGHT",       flags & POINT_LIGHT       ? "1" : "0" },
			{ nullptr, nullptr }
		};
		iter.first->second->Create("../Assets/Shader/Default.hlsl", "VS", "PS", macros);
	}
	return iter.first->second.get();
}
