#pragma once
#include "Resource.h"

class Animation final : public Resource<Animation>
{
	struct BoneAnimationKeyframe
	{
		float      mTime;
		Vector3    mScale;
		Quaternion mRotation;
		Vector3    mPosition;
	};
	struct BoneAnimation
	{
		std::string mBoneName;
		std::vector<BoneAnimationKeyframe> mKeyframes;
	};

public:
	Animation(class Context* context);
	~Animation() = default;

	void LoadFromFile(const std::string& path) override;

	const std::vector<BoneAnimation>& GetBoneAnimations() const { return mBoneAnims; }

private:
	std::vector<BoneAnimation> mBoneAnims;
};