#include "Framework.h"
#include "Animator.h"

Animator::Animator(Context * context, GameObject * owner) :
	Component<Animator>(context, owner),
	mAnimation(nullptr)
{
}

void Animator::Update()
{
}

void Animator::SetAnimation(Animation * animation)
{
	mAnimation = animation;

	if (mAnimation)
	{
		auto& bones = mAnimation->GetBoneAnimations();
		mBones.resize(bones.size());
		for (size_t i = 0; i < bones.size(); i++)
			mBones[i].mBoneName = bones[i].mBoneName;
	}
}