#include "Framework.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Context* context, GameObject* owner) :
	Component<MeshRenderer>(context, owner),
	m_texture(nullptr),
	m_mesh(nullptr)
{
	m_transform = owner->GetComponent<Transform>();
}

MeshRenderer::MeshRenderer(const MeshRenderer & rhs) :
	Component<MeshRenderer>(rhs.m_context, nullptr),
	m_texture(rhs.m_texture),
	m_mesh(rhs.m_mesh)
{
}

void MeshRenderer::Update()
{
}