#include "Framework.h"
#include "Animation.h"

Animation::Animation(Context * context) :
	Resource<Animation>(context)
{
}

void Animation::LoadFromFile(const std::string & path)
{
	FileStreamRead r(path);

	r.Read(mFramerate);
	r.Read(mDuration);

	for (uint8_t i = 0; i < r.Read<uint8_t>(); i++)
	{
		mChannels[r.Read<std::string>()] = r.Read<BoneAnimation>();
	}
}

void Animation::SaveToFile(const std::string & path)
{
	FileStreamWrite w(path);

	w.Write(mFramerate);
	w.Write(mDuration);

	w.Write<uint8_t>(mChannels.size());
	for (auto channel : mChannels)
	{
		w.Write(channel.first);
		w.Write(channel.second);
	}
}

Animation::BoneAnimation & Animation::AddChannel(const std::string & name)
{
	return mChannels[name];
}

Matrix Animation::EvaluateTransform(const std::string & name, float time)
{
	auto iter = mChannels.find(name);
	if (iter == mChannels.end())
		return DirectX::XMMatrixIdentity();

	unsigned int index = static_cast<unsigned int>(time * mFramerate);
	if (index > iter->second.size())
		return DirectX::XMMatrixIdentity();

	BoneKeyframe keyframe0 = iter->second[index];
	BoneKeyframe keyframe1 = iter->second[index + 1];

	float blend = (time - keyframe0.mTime) / (keyframe1.mTime - keyframe0.mTime);

	DirectX::XMVECTOR scale    = DirectX::XMVectorLerp(keyframe0.mScale, keyframe1.mScale, blend);
	DirectX::XMVECTOR rotation = DirectX::XMQuaternionSlerp(keyframe0.mRotation, keyframe1.mRotation, blend);
	DirectX::XMVECTOR position = DirectX::XMVectorLerp(keyframe0.mPosition, keyframe1.mPosition, blend);

	return DirectX::XMMatrixAffineTransformation(
		scale, DirectX::XMVectorSet(0, 0, 0, 1), rotation, position);
}
