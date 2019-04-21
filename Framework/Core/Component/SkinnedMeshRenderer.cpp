#include "Framework.h"
#include "SkinnedMeshRenderer.h"

SkinnedMeshRenderer::SkinnedMeshRenderer(Context * context, GameObject * owner) :
	Component<SkinnedMeshRenderer>(context, owner),
	mRootTransform(nullptr),
	mMaterial(nullptr),
	mMesh(nullptr)
{
}

void SkinnedMeshRenderer::Update()
{
}
