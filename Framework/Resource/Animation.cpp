#include "Framework.h"
#include "Animation.h"

Animation::Animation(Context * context) :
	Resource<Animation>(context)
{
}

void Animation::LoadFromFile(const std::string & path)
{
}
