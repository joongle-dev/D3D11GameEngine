#include "Framework.h"
#include "Light.h"

Light::Light(Context * context, GameObject * owner) :
	Component<Light>(context, owner),
	m_eLightType(Directional),
	m_vLightColor(1.0f, 1.0f, 1.0f, 1.0f),
	m_fLightRange(1.0f)
{
	m_pTransform = m_pGameObject->GetComponent<Transform>();
}

void Light::Update()
{
}