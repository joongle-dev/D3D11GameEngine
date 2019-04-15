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
	using BoneAnimation = std::pair<std::string, std::vector<BoneAnimationKeyframe>>;

public:
	Animation(class Context* context);
	~Animation() = default;

	void LoadFromFile(const std::string& path) override;
	void SaveToFile(const std::string& path) override;

private:
	std::map<std::string, std::vector<BoneAnimationKeyframe>> mBoneAnims;
};