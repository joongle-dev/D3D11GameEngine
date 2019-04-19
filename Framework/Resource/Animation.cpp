#include "Framework.h"
#include "Animation.h"

Animation::Animation(Context * context) :
	Resource<Animation>(context)
{
}

void Animation::LoadFromFile(const std::string & path)
{
	FileStreamRead r(path);

	for (uint8_t i = 0; i < r.Read<uint8_t>(); i++)
	{
		BoneAnimation& channel = mChannels[r.Read<std::string>()];
		r.Read(channel);
	}
}

void Animation::SaveToFile(const std::string & path)
{
	FileStreamWrite w(path);

	w.Write<uint8_t>(mChannels.size());
	for (auto channel : mChannels)
	{
		w.Write(channel.first);
		w.Write(channel.second);
	}
}

Animation::BoneAnimation * Animation::AddChannel(const std::string & name)
{
	return &mChannels[name];
}

Animation::BoneAnimation * Animation::GetChannel(const std::string & name)
{
	auto iter = mChannels.find(name);
	if (iter != mChannels.end())
		return &iter->second;
	return nullptr;
}