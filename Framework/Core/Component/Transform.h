#pragma once
#include "Component.h"

class Transform final : public Component<Transform>
{
	using Transforms = std::vector<Transform*>;

public:
	Transform(class Context* context, class GameObject* owner);
	~Transform();

	Transform(const Transform& rhs);
	Transform& operator=(const Transform&) = delete;

	void Update() override;

	//Local transform mutator and accessors
	void SetLocalScale(const Vector3 vec) { m_localScale = vec; SetUpdateFlag(); }
	void SetLocalRotation(const Quaternion quat) { m_localRotation = quat; SetUpdateFlag(); }
	void SetLocalPosition(const Vector3 vec) { m_localPosition = vec; SetUpdateFlag(); }
	const Vector3& GetLocalScale() const { return m_localScale; }
	const Quaternion& GetLocalRotation() const { return m_localRotation; }
	const Vector3& GetLocalPosition() const { return m_localPosition; }

	//World transform mutators and accessors
	void SetScale(const Vector3 vec);
	void SetRotation(const Quaternion quat);
	void SetPosition(const Vector3 vec);
	const Vector3 GetScale();
	const Quaternion GetRotation();
	const Vector3 GetPosition();

	//Matrix accessors
	const Matrix& GetLocalTransform() { Update(); return m_localMatrix; }
	const Matrix& GetWorldTransform() { Update(); return m_worldMatrix; }

	//Transform mutators
	void RotateAxis(Vector3 axis, float angle);
	void RotateEuler(Vector3 angles);
	void SetRotationEuler(Vector3 angles);
	void Translate(Vector3 translation);

	//Directional vectors
	const Vector3 GetForward() { return Vector3::Transform(Vector3(0, 0, 1), GetRotation()); }
	const Vector3 GetRight() { return Vector3::Transform(Vector3(1, 0, 0), GetRotation()); }
	const Vector3 GetUp() { return Vector3::Transform(Vector3(0, 1, 0), GetRotation()); }
	
	//Hierarchy accessors and mutators
	const bool HasParent() const { return m_parent ? true : false; }
	const bool HasChild() const { return !m_children.empty(); }
	void SetParent(Transform* parent);
	Transform* GetParent() const { return m_parent; }
	const size_t GetChildCount() const { return m_children.size(); }
	void AddChild(Transform* child) { child->SetParent(this); }
	Transform* GetChild(size_t index) const { return index < m_children.size() ? m_children[index] : nullptr;}

	const int GetTest() { return 0; }
	void SetTest(const int a) { return; }

private:
	void RemoveChild(Transform* child);
	void SetUpdateFlag();

private:
	void RegisterSerializeFields() override
	{
		RegisterSerializeField("Position", &Transform::GetTest, &Transform::SetTest);
	}

private:
	Vector3		m_localScale;
	Quaternion	m_localRotation;
	Vector3		m_localPosition;
	Matrix		m_localMatrix;
	Matrix		m_worldMatrix;
	
	bool		m_dirty;
	Transform*	m_parent;
	Transforms	m_children;
};