#pragma once
#include "Component.h"

class Light final : public Component<Light>
{
	enum LightType : unsigned char
	{
		Directional,
		Point,
		Spot,
	};

public:
	Light(class Context* context, class GameObject* owner);
	~Light() = default;

	void Update() override;

private:
	Transform* m_pTransform;

	LightType m_eType;
	Color m_vColor;
	
};