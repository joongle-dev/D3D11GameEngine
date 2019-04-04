#include "Framework.h"
#include "Renderer.h"
#include "VertexBuffer.h"

using namespace DirectX;

Renderer::Renderer(Context * context) :
	Subsystem<Renderer>(context), m_camera(nullptr)
{
	m_graphics = context->GetSubsystem<Graphics>();

	m_mainTarget = new RenderTarget(context);
	m_mainTarget->Create(1920, 1080);

	m_CameraBuffer = new ConstantBuffer<CameraBuffer>(context);
	m_WorldBuffer = new ConstantBuffer<WorldBuffer>(context);
	m_LightBuffer = new ConstantBuffer<LightBuffer>(context);

	D3D11_INPUT_ELEMENT_DESC Desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_layout = new InputLayout(context);
	m_layout->Create(GetMatchingShader(0)->GetBytecode(), Desc, sizeof(Desc) / sizeof(Desc[0]));

	BlendState* blend = new BlendState(context);
	blend->Create();
	blend->Bind();

	DepthStencilState* depth = new DepthStencilState(context);
	depth->Create();
	depth->Bind();

	RasterizerState* raster = new RasterizerState(context);
	raster->Create();
	raster->Bind();


	Geometry geom;
	Geometry::CreateCube(geom);
	geom.GenerateTangents();
	std::vector<GizmoVertex> vertices;
	for (size_t i = 0; i < geom.positions.size(); i++)
	{
		vertices.push_back({ geom.positions[i], Vector4(1, 0, 0, 1) });
		vertices.push_back({ geom.positions[i] + geom.normals[i] * 0.25f, Vector4(1, 0, 0, 1) });
		vertices.push_back({ geom.positions[i], Vector4(0, 1, 0, 1) });
		vertices.push_back({ geom.positions[i] + geom.tangents[i] * 0.25f, Vector4(0, 1, 0, 1) });
		vertices.push_back({ geom.positions[i], Vector4(0, 0, 1, 1) });
		vertices.push_back({ geom.positions[i] + geom.binormals[i] * 0.25f, Vector4(0, 0, 1, 1) });
	}
	gizmovertices = new VertexBuffer(context);
	gizmovertices->Create<GizmoVertex>(vertices);

	gizmoshader = new Shader(context);
	gizmoshader->Create("../Assets/Shader/Gizmo.hlsl");

	gizmolayout = new InputLayout(context);
	gizmolayout->Create(gizmoshader->GetBytecode());
}

void Renderer::Update()
{
	auto pCurrentScene = m_context->GetSubsystem<SceneManager>()->GetCurrentScene();

	m_layout->Bind();

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
	auto pCameraData = m_CameraBuffer->Map();
	{
		pCameraData->view = XMMatrixTranspose(camera->GetViewMatrix());
		pCameraData->projection = XMMatrixTranspose(camera->GetProjectionMatrix());
		pCameraData->campos = camera->GetTransform()->GetPosition();
	}
	m_CameraBuffer->Unmap();
	m_CameraBuffer->Bind(ShaderType::VS, 0);

	UINT stride[] = { sizeof(Vector3), sizeof(Vector3), sizeof(Vector3), sizeof(Vector2) };
	UINT offset[] = { 0, 0, 0, 0 };

	for (auto itRenderable = scene->ComponentBegin<MeshRenderer>(); itRenderable != scene->ComponentEnd<MeshRenderer>(); itRenderable++)
	{
		Transform* pRenderableTransform = itRenderable->GetTransform();
		auto pWorldData = m_WorldBuffer->Map();
		{
			pWorldData->world = XMMatrixTranspose(pRenderableTransform->GetWorldTransform());
		}
		m_WorldBuffer->Unmap();
		Mesh* pMesh = itRenderable->GetMesh();
		m_WorldBuffer->Bind(ShaderType::VS, 1);

		if (!pMesh) continue;

		Material* pMaterial = itRenderable->GetMaterial();
		if (!pMaterial) continue;

		pMaterial->GetShader()->Bind();

		ID3D11ShaderResourceView* srv[] =
		{
			pMaterial->GetTexture(Material::TEX_ALBEDO) ? pMaterial->GetTexture(Material::TEX_ALBEDO)->GetTexture() : nullptr,
			pMaterial->GetTexture(Material::TEX_ROUGHNESS) ? pMaterial->GetTexture(Material::TEX_ROUGHNESS)->GetTexture() : nullptr,
			pMaterial->GetTexture(Material::TEX_METALLIC) ? pMaterial->GetTexture(Material::TEX_METALLIC)->GetTexture() : nullptr,
			pMaterial->GetTexture(Material::TEX_NORMAL) ? pMaterial->GetTexture(Material::TEX_NORMAL)->GetTexture() : nullptr,
			pMaterial->GetTexture(Material::TEX_HEIGHT) ? pMaterial->GetTexture(Material::TEX_HEIGHT)->GetTexture() : nullptr
		};

		ID3D11Buffer* vbs[] =
		{
			pMesh->m_positions.Get(),
			pMesh->m_normals.Get(),
			pMesh->m_tangents.Get(),
			pMesh->m_uvs.Get()
		};

		m_graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_layout->Bind();
		m_graphics->GetDeviceContext()->IASetVertexBuffers(0, 4, vbs, stride, offset);
		m_graphics->GetDeviceContext()->IASetIndexBuffer(pMesh->m_indices.Get(), DXGI_FORMAT_R32_UINT, 0);

		m_graphics->GetDeviceContext()->PSSetShaderResources(0, 5, srv);
		for (auto itLight = scene->ComponentBegin<Light>(); itLight != scene->ComponentEnd<Light>(); itLight++)
		{
			Transform* pLightTransform = itLight->GetTransform();
			auto pLightData = m_LightBuffer->Map();
			{
				pLightData->lightpos = pLightTransform->GetPosition();
				pLightData->lightdir = pLightTransform->GetForward();
			}
			m_LightBuffer->Unmap();
			m_LightBuffer->Bind(ShaderType::VS, 2);

			m_graphics->GetDeviceContext()->DrawIndexed(pMesh->GetIndexCount(), 0, 0);
		}

		gizmoshader->Bind();
		gizmolayout->Bind();
		m_graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		gizmovertices->BindPipeline();
		m_graphics->GetDeviceContext()->Draw(24, 0);
	}
}

Shader * Renderer::GetMatchingShader(unsigned short flags)
{
	auto iter = m_shaders.emplace(flags, new Shader(m_context));
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
		iter.first->second->test = flags;
	}
	return iter.first->second;
}
