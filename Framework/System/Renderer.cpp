#include "Framework.h"
#include "Renderer.h"

using namespace DirectX;

Renderer::Renderer(Context * context) :
	Subsystem<Renderer>(context), m_camera(nullptr)
{
	m_graphics = context->GetSubsystem<Graphics>();

	m_cameraBuffer = new ConstantBuffer<CameraBuffer>(context);
	m_worldBuffer = new ConstantBuffer<WorldBuffer>(context);

	m_shader = new Shader(context);
	m_shader->Create("MeshTest.hlsl");

	m_layout = new InputLayout(context);
	m_layout->Create(m_shader->GetBytecode());

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
	auto componentmanager = m_context->GetSubsystem<ComponentManager>();

	m_layout->Bind();
	m_shader->Bind();

	for (auto camera = componentmanager->begin<Camera>(); camera != componentmanager->end<Camera>(); camera++)
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
		
		for (auto renderable = componentmanager->begin<MeshRenderer>(); renderable != componentmanager->end<MeshRenderer>(); renderable++)
		{
			auto worldData = m_worldBuffer->Map();
			{
				worldData->world = XMMatrixTranspose(XMLoadFloat4x4(&renderable->GetTransform()->GetWorldTransform()));
			}
			m_worldBuffer->Unmap();
			m_worldBuffer->Bind(ShaderType::VS, 1);

			Transform* trans = renderable->GetTransform();
			Mesh* mesh = renderable->GetMesh();

			UINT stride = sizeof(Vector3);
			UINT offset = 0;

			m_graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh->m_positions.GetAddressOf(), &stride, &offset);
			m_graphics->GetDeviceContext()->IASetIndexBuffer(mesh->m_indices.Get(), DXGI_FORMAT_R32_UINT, 0);
			m_graphics->GetDeviceContext()->DrawIndexed(mesh->GetIndexCount(), 0, 0);
		}
	}
}