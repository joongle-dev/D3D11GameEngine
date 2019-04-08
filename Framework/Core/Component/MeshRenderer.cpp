#include "Framework.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Context* context, GameObject* owner) :
	Component<MeshRenderer>(context, owner),
	mMaterial(nullptr),
	mMesh(nullptr)
{
	mTransform = owner->GetComponent<Transform>();
}

MeshRenderer::MeshRenderer(const MeshRenderer & rhs) :
	Component<MeshRenderer>(rhs.m_context, nullptr),
	mMaterial(rhs.mMaterial),
	mMesh(rhs.mMesh)
{
}

void MeshRenderer::Update()
{
}