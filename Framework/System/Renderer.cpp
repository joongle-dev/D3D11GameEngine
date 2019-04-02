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

	m_cameraBuffer = new ConstantBuffer<CameraBuffer>(context);
	m_worldBuffer = new ConstantBuffer<WorldBuffer>(context);

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
	auto scene = m_context->GetSubsystem<SceneManager>()->GetCurrentScene();

	m_layout->Bind();

	for (auto camera = scene->ComponentBegin<Camera>(); camera != scene->ComponentEnd<Camera>(); camera++)
		RenderCamera(scene, camera);
}

Shader * Renderer::GetMatchingShader(unsigned short flags)
{
	auto iter = m_shaders.emplace(flags, new Shader(m_context));
	if (iter.second)
	{
		D3D_SHADER_MACRO macros[] = {
			{ "ALBEDO",    flags & TEX_ALBEDO    ? "1" : "0" },
			{ "ROUGHNESS", flags & TEX_ROUGHNESS ? "1" : "0" },
			{ "METALLIC",  flags & TEX_METALLIC  ? "1" : "0" },
			{ "NORMAL",    flags & TEX_NORMAL    ? "1" : "0" },
			{ "HEIGHT",    flags & TEX_HEIGHT    ? "1" : "0" },
			{ nullptr, nullptr }
		};
		iter.first->second->Create("../Assets/Shader/Default.hlsl", "VS", "PS", macros);
		iter.first->second->test = flags;
	}
	return iter.first->second;
}

void Renderer::RenderCamera(Scene* scene, Camera * camera)
{
	RenderTarget* pRenderTarget = camera->GetRenderTarget();

	if (!pRenderTarget)
		return;

	pRenderTarget->Clear(Color(0.5f, 0.5f, 0.0f, 1.0f));
	pRenderTarget->Bind();

	auto pCameraData = m_cameraBuffer->Map();
	{
		pCameraData->view = XMMatrixTranspose(XMLoadFloat4x4(&camera->GetViewMatrix()));
		pCameraData->projection = XMMatrixTranspose(XMLoadFloat4x4(&camera->GetProjectionMatrix()));
		pCameraData->campos = camera->GetTransform()->GetPosition();
	}
	m_cameraBuffer->Unmap();
	m_cameraBuffer->Bind(ShaderType::VS, 0);

	UINT stride[] = { sizeof(Vector3), sizeof(Vector3), sizeof(Vector3), sizeof(Vector2) };
	UINT offset[] = { 0, 0, 0, 0 };

	for (auto renderable = scene->ComponentBegin<MeshRenderer>(); renderable != scene->ComponentEnd<MeshRenderer>(); renderable++)
	{
		auto pWorldData = m_worldBuffer->Map();
		{
			pWorldData->world = XMMatrixTranspose(XMLoadFloat4x4(&renderable->GetTransform()->GetWorldTransform()));
		}
		m_worldBuffer->Unmap();
		m_worldBuffer->Bind(ShaderType::VS, 1);

		Mesh* pMesh = renderable->GetMesh();
		if (!pMesh) continue;

		Material* pMaterial = renderable->GetMaterial();
		if (!pMaterial) continue;

		pMaterial->GetShader()->Bind();

		ID3D11ShaderResourceView* srv[] =
		{
			pMaterial->GetTexture(TEX_ALBEDO) ? pMaterial->GetTexture(TEX_ALBEDO)->GetTexture() : nullptr,
			pMaterial->GetTexture(TEX_ROUGHNESS) ? pMaterial->GetTexture(TEX_ROUGHNESS)->GetTexture() : nullptr,
			pMaterial->GetTexture(TEX_METALLIC) ? pMaterial->GetTexture(TEX_METALLIC)->GetTexture() : nullptr,
			pMaterial->GetTexture(TEX_NORMAL) ? pMaterial->GetTexture(TEX_NORMAL)->GetTexture() : nullptr,
			pMaterial->GetTexture(TEX_HEIGHT) ? pMaterial->GetTexture(TEX_HEIGHT)->GetTexture() : nullptr
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
		m_graphics->GetDeviceContext()->DrawIndexed(pMesh->GetIndexCount(), 0, 0);

		gizmoshader->Bind();
		gizmolayout->Bind();
		m_graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		gizmovertices->BindPipeline();
		m_graphics->GetDeviceContext()->Draw(24, 0);
	}
}
