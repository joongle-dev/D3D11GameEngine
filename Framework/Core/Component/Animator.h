#pragma once
#include "Component.h"

class Animator final : public Component<Animator>
{
	struct Bone
	{
		std::string mBoneName;
		class Transform* mTransform;
	};
public:
	Animator(class Context* context, class GameObject* owner);
	~Animator() = default;

	void Update() override;

	class Animation* GetAnimation() const { return mAnimation; }
	void SetAnimation(class Animation* animation);

	const std::vector<Bone>& GetBones() const { return mBones; }
	void SetBone(size_t index, class Transform* bone) { mBones[index].mTransform = bone; }

private:
	class Animation* mAnimation;

	std::vector<Bone> mBones;
};