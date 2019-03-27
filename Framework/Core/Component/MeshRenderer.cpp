#include "Framework.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Context* context, GameObject* owner) :
	Component<MeshRenderer>(context, owner),
	m_material(nullptr),
	m_mesh(nullptr)
{
	m_transform = owner->GetComponent<Transform>();
}

MeshRenderer::MeshRenderer(const MeshRenderer & rhs) :
	Component<MeshRenderer>(rhs.m_context, nullptr),
	m_material(rhs.m_material),
	m_mesh(rhs.m_mesh)
{
}

void MeshRenderer::Update()
{
}