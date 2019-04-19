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
	using BoneAnimation = std::vector<BoneAnimationKeyframe>;

public:
	Animation(class Context* context);
	~Animation() = default;

	void LoadFromFile(const std::string& path) override;
	void SaveToFile(const std::string& path) override;

	BoneAnimation* AddChannel(const std::string& name);
	BoneAnimation* GetChannel(const std::string& name);


private:
	float mFramerate;
	std::map<std::string, BoneAnimation> mChannels;
};