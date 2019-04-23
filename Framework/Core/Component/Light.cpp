#include "Framework.h"
#include "Light.h"

Light::Light(Context * context, GameObject * owner) :
	Component<Light>(context, owner),
	mType(Directional),
	mColor(1.0f, 1.0f, 1.0f),
	mRange(1.0f),
	mIntensity(0.75f),
	mSpotAngle(PI_DIV_4)
{
	mTransform = m_pGameObject->GetComponent<Transform>();
}

void Light::Update()
{
}