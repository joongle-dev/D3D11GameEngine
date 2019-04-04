#pragma once
#include "Component.h"

class Light final : public Component<Light>
{
public:
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

	void SetLightType(const LightType type) { m_eType = type; }
	const LightType GetLightType() const { return m_eType; }

	void SetColor(const Color& color) { m_vColor = color; }
	const Color& GetColor() const { return m_vColor; }

	void SetRange(const float range) { m_fRange = range; }
	const float GetRange() const { return m_fRange; }

	class Transform* GetTransform() const { return m_pTransform; }

private:
	class Transform* m_pTransform;

	LightType m_eType;
	Color m_vColor;
	float m_fRange;
};