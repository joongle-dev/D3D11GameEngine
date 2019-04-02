#include "Framework.h"
#include "Light.h"

Light::Light(Context * context, GameObject * owner) :
	Component<Light>(context, owner),
	m_eType(Directional)
{
	m_pTransform = owner->GetComponent<Transform>();
}

void Light::Update()
{
}