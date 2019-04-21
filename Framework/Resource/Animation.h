#pragma once
#include "Resource.h"

class Animation final : public Resource<Animation>
{
public:
	struct BoneKeyframe
	{
		float      mTime;
		Vector3    mScale;
		Quaternion mRotation;
		Vector3    mPosition;
	};
	using BoneAnimation = std::vector<BoneKeyframe>;

public:
	Animation(class Context* context);
	~Animation() = default;

	void LoadFromFile(const std::string& path) override;
	void SaveToFile(const std::string& path) override;

	float GetFramerate() const { return mFramerate; }
	void SetFramerate(float framerate) { mFramerate = framerate; }

	float GetDuration() const { return mDuration; }
	void SetDuration(float duration) { mDuration = duration; }

	BoneAnimation& AddChannel(const std::string& name);
	Matrix EvaluateTransform(const std::string& name, float time);

private:
	float mFramerate;
	float mDuration;
	std::map<std::string, BoneAnimation> mChannels;
};