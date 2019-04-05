#include "Framework.h"
#include "Renderer.h"
#include "VertexBuffer.h"

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

	BlendState* blend = new BlendState(context);
	blend->Create();
	blend->Bind();

	DepthStencilState* depth = new DepthStencilState(context);
	depth->Create();
	depth->Bind();

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

	pRenderTarget->Clear(Color(0.0f, 0.0f, 0.0f, 1.0f));
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
			pMesh->GetNormals(),
			pMesh->GetTangents(),
			pMesh->GetTexcoords(),
		};

		Material* pMaterial = itRenderable->GetMaterial();
		if (!pMaterial) continue;

		unsigned char materialflags = pMaterial->GetMaterialFlags();

		GetMatchingShader(materialflags)->Bind();

		ID3D11ShaderResourceView* srv[] =
		{
			materialflags & Material::TEX_ALBEDO    ? pMaterial->GetTexture(Material::TEX_ALBEDO)->GetTexture()    : nullptr,
			materialflags & Material::TEX_ROUGHNESS ? pMaterial->GetTexture(Material::TEX_ROUGHNESS)->GetTexture() : nullptr,
			materialflags & Material::TEX_METALLIC  ? pMaterial->GetTexture(Material::TEX_METALLIC)->GetTexture()  : nullptr,
			materialflags & Material::TEX_NORMAL    ? pMaterial->GetTexture(Material::TEX_NORMAL)->GetTexture()    : nullptr,
			materialflags & Material::TEX_HEIGHT    ? pMaterial->GetTexture(Material::TEX_HEIGHT)->GetTexture()    : nullptr,
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

			mGraphics->GetDeviceContext()->DrawIndexed(pMesh->GetIndexCount(), 0, 0);
		}
	}
}

Shader * Renderer::GetMatchingShader(unsigned char flags)
{
	auto iter = mShaders.emplace(flags, std::make_unique<Shader>(m_context));
	if (iter.second)
	{
		D3D_SHADER_MACRO macros[] = {
			{ "ALBEDO",    flags & Material::TEX_ALBEDO    ? "1" : "0" },
			{ "ROUGHNESS", flags & Material::TEX_ROUGHNESS ? "1" : "0" },
			{ "METALLIC",  flags & Material::TEX_METALLIC  ? "1" : "0" },
			{ "NORMAL",    flags & Material::TEX_NORMAL    ? "1" : "0" },
			{ "HEIGHT",    flags & Material::TEX_HEIGHT    ? "1" : "0" },
			{ nullptr, nullptr }
		};
		iter.first->second->Create("../Assets/Shader/Default.hlsl", "VS", "PS", macros);
	}
	return iter.first->second.get();
}
