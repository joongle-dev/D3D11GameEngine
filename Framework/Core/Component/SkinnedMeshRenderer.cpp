#include "Framework.h"
#include "SkinnedMeshRenderer.h"

SkinnedMeshRenderer::SkinnedMeshRenderer(Context * context, GameObject * owner) :
	Component<SkinnedMeshRenderer>(context, owner),
	mRootTransform(nullptr),
	mMaterial(nullptr),
	mMesh(nullptr)
{
	mTransform = owner->GetComponent<Transform>();
}

void SkinnedMeshRenderer::Update()
{
}
