#pragma once
#include "Component.h"

class Light final : public Component<Light>
{
public:
	enum LightType : unsigned int
	{
		Directional,
		Point,
		Spot,
	};

public:
	Light(class Context* context, class GameObject* owner);
	~Light() = default;

	void Update() override;

	void SetLightType(const LightType type) { mType = type; }
	const LightType GetLightType() const { return mType; }

	void SetColor(const Vector3& color) { mColor = color; }
	const Vector3& GetColor() const { return mColor; }

	void SetRange(const float range) { mRange = range; }
	const float GetRange() const { return mRange; }

	void SetIntensity(const float intensity) { mIntensity = intensity; }
	const float GetIntensity() const { return mIntensity; }

	void SetSpotAngle(const float angle) { mSpotAngle = angle; }
	const float GetSpotAngle() const { return mSpotAngle; }

	class Transform* GetTransform() const { return mTransform; }

private:
	class Transform* mTransform;

	LightType mType;
	Vector3   mColor;
	float     mRange;
	float     mIntensity;
	float     mSpotAngle;
};