#include "Framework.h"
#include "Renderer.h"

using namespace DirectX;

Renderer::Renderer(Context * context) :
	Subsystem<Renderer>(context), m_camera(nullptr)
{
	m_graphics = context->GetSubsystem<Graphics>();

	m_mainTarget = new RenderTarget(context);
	m_mainTarget->Create(1920, 1080);

	m_cameraBuffer = new ConstantBuffer<CameraBuffer>(context);
	m_worldBuffer = new ConstantBuffer<WorldBuffer>(context);

	m_shader = new Shader(context);
	m_shader->Create("../Assets/Shader/Default.hlsl");

	D3D11_INPUT_ELEMENT_DESC Desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_layout = new InputLayout(context);
	m_layout->Create(m_shader->GetBytecode(), Desc, 3);

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
	auto scene = m_context->GetSubsystem<SceneManager>()->GetCurrentScene();

	m_layout->Bind();
	m_shader->Bind();

	for (auto camera = scene->ComponentBegin<Camera>(); camera != scene->ComponentEnd<Camera>(); camera++)
	{
		RenderTarget* rendertarget = camera->GetRenderTarget();

		if (!rendertarget) 
			continue;

		rendertarget->Clear(Color(0, 0, 0, 1));
		rendertarget->Bind();

		auto cameraData = m_cameraBuffer->Map();
		{
			cameraData->view = XMMatrixTranspose(XMLoadFloat4x4(&camera->GetViewMatrix()));
			cameraData->projection = XMMatrixTranspose(XMLoadFloat4x4(&camera->GetProjectionMatrix()));
		}
		m_cameraBuffer->Unmap();
		m_cameraBuffer->Bind(ShaderType::VS, 0);
		
		for (auto renderable = scene->ComponentBegin<MeshRenderer>(); renderable != scene->ComponentEnd<MeshRenderer>(); renderable++)
		{
			auto worldData = m_worldBuffer->Map();
			{
				worldData->world = XMMatrixTranspose(XMLoadFloat4x4(&renderable->GetTransform()->GetWorldTransform()));
			}
			m_worldBuffer->Unmap();
			m_worldBuffer->Bind(ShaderType::VS, 1);

			Mesh* mesh = renderable->GetMesh();

			UINT stride[] = { sizeof(Vector3), sizeof(Vector3), sizeof(Vector2) };
			UINT offset[] = { 0, 0, 0 };
			ID3D11Buffer* vbs[] = { mesh->m_positions.Get(), mesh->m_normals.Get(), mesh->m_uvs.Get() };

			m_graphics->GetDeviceContext()->IASetVertexBuffers(0, 3, vbs, stride, offset);
			m_graphics->GetDeviceContext()->IASetIndexBuffer(mesh->m_indices.Get(), DXGI_FORMAT_R32_UINT, 0);
			m_graphics->GetDeviceContext()->DrawIndexed(mesh->GetIndexCount(), 0, 0);
		}
	}
}