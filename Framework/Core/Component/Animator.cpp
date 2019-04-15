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
}