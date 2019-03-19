#include "Framework.h"
#include "Transform.h"
#include "../GameObject.h"

using namespace DirectX;

Transform::Transform(Context* context, GameObject* owner) :
	Component<Transform>(context, owner),
	m_localScale(1, 1, 1),
	m_localRotation(0, 0, 0, 1),
	m_localPosition(0, 0, 0),
	m_parent(nullptr),
	m_dirty(true) {}

Transform::~Transform()
{
	auto owner = GetOwner();
	//Destroy all transforms' GameObjects
	for (auto child : m_children)
		child->GetOwner()->Destroy();

	//Remove self from parent's children
	if (m_parent)
		m_parent->RemoveChild(this);
}

Transform::Transform(const Transform & rhs) :
	Component<Transform>(rhs.m_context, nullptr)
{
	m_localScale = rhs.m_localScale;
	m_localRotation = rhs.m_localRotation;
	m_localPosition = rhs.m_localPosition;
}

void Transform::Update()
{
	//If update flag is true
	if (m_dirty)
	{
		//Create new local transformation matrix
		m_localMatrix = XMMatrixAffineTransformation(m_localScale, XMVectorSet(0, 0, 0, 1), m_localRotation, m_localPosition);

		//Create new world transformation matrix
		if (m_parent)
			m_worldMatrix = XMMatrixMultiply(m_localMatrix, m_parent->GetWorldTransform());
		else
			m_worldMatrix = m_localMatrix;

		//Turn off update flag
		m_dirty = false;
	}
}

void Transform::SetScale(const Vector3 vec)
{
	if (m_parent) {
		Vector3 parent = m_parent->GetScale();
		m_localScale.x = vec.x / parent.x;
		m_localScale.y = vec.y / parent.y;
		m_localScale.z = vec.x / parent.z;
	}
	else
		m_localScale = vec;
	
	SetUpdateFlag();
}

void Transform::SetRotation(const Quaternion quat)
{
	if (m_parent)
		m_localRotation = XMQuaternionMultiply(quat, XMQuaternionInverse(m_parent->GetRotation()));
	else
		m_localRotation = quat;

	SetUpdateFlag();
}

void Transform::SetPosition(const Vector3 vec)
{
	if (m_parent)
		m_localPosition = XMVector3TransformCoord(vec, XMMatrixInverse(nullptr, m_parent->GetWorldTransform()));
	else
		m_localPosition = vec;

	SetUpdateFlag();
}

const Vector3 Transform::GetScale()
{
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, GetWorldTransform());
	return scale;
}

const Quaternion Transform::GetRotation()
{
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, GetWorldTransform());
	return rotation;
}

const Vector3 Transform::GetPosition()
{
	return GetWorldTransform().Translation();
}

const Vector3 Transform::GetEulerRotation()
{
	Quaternion q = GetRotation();

	float check = 2.0f * (-q.y * q.z + q.w * q.x);

	if (check < -0.995f)
		return Vector3(
			-90.0f,
			0.0f,
			-atan2f(2.0f * (q.x * q.z - q.w * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)) * TO_DEG
		);

	if (check > 0.995f)
		return Vector3(
			90.0f,
			0.0f,
			atan2f(2.0f * (q.x * q.z - q.w * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)) * TO_DEG
		);

	return Vector3(
		asinf(check) * TO_DEG,
		atan2f(2.0f * (q.x * q.z + q.w * q.y), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)) * TO_DEG,
		atan2f(2.0f * (q.x * q.y + q.w * q.z), 1.0f - 2.0f * (q.x * q.x + q.z * q.z)) * TO_DEG
	);
}

void Transform::RotateAxis(Vector3 axis, float angle)
{
	Quaternion quat = XMQuaternionMultiply(GetRotation(), XMQuaternionRotationAxis(axis, angle));
	SetRotation(quat);
}

void Transform::RotateEuler(Vector3 angles)
{
	angles *= TO_RAD;
	Quaternion quat = XMQuaternionMultiply(GetRotation(), XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z));
	SetRotation(quat);
}

void Transform::SetEulerRotation(Vector3 angles)
{
	angles *= TO_RAD;
	Quaternion quat = XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z);
	SetRotation(quat);
}

void Transform::Translate(Vector3 translation)
{
	Vector3 vec = GetPosition() + translation;
	SetPosition(vec);
}

void Transform::SetParent(Transform * parent)
{
	//Remove self from parent's children
	if (m_parent)
  		m_parent->RemoveChild(this);

	//Change parent and add self to new parent's children
	m_parent = parent;
	parent->m_children.push_back(this);

	SetUpdateFlag();
}

void Transform::RemoveChild(Transform * child)
{
	//Search and remove child
	for (auto iter = m_children.begin(); iter != m_children.end(); iter++)
		if (*iter == child) { 
			m_children.erase(iter); 
			return; }
}

void Transform::SetUpdateFlag()
{
	m_dirty = true;
	for (auto child : m_children)
		child->SetUpdateFlag();
}