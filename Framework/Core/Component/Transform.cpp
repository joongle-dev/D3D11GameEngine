#include "Framework.h"
#include "Transform.h"
#include "../GameObject.h"

using namespace DirectX;

Transform::Transform(Context* context, GameObject* owner) :
	Component<Transform>(context, owner),
	mLocalScale(1, 1, 1),
	mLocalRotation(0, 0, 0, 1),
	mLocalPosition(0, 0, 0),
	mParent(nullptr),
	mIsChanged(true) {}

Transform::~Transform()
{
	//Remove self from parent's children
	if (mParent)
		mParent->RemoveChild(this);
}

Transform::Transform(const Transform & rhs) :
	Component<Transform>(rhs.m_context, nullptr)
{
	mLocalScale = rhs.mLocalScale;
	mLocalRotation = rhs.mLocalRotation;
	mLocalPosition = rhs.mLocalPosition;
}

void Transform::Update()
{
	//If update flag is true
	if (mIsChanged)
	{
		//Create new local transformation matrix
		mLocalMatrix = XMMatrixAffineTransformation(mLocalScale, XMVectorSet(0, 0, 0, 1), mLocalRotation, mLocalPosition);

		//Create new world transformation matrix
		if (mParent)
			mWorldMatrix = XMMatrixMultiply(mLocalMatrix, mParent->GetWorldTransform());
		else
			mWorldMatrix = mLocalMatrix;

		//Turn off update flag
		mIsChanged = false;
	}
}

void Transform::SetScale(const Vector3 vec)
{
	if (mParent) {
		Vector3 parent = mParent->GetScale();
		mLocalScale.x = vec.x / parent.x;
		mLocalScale.y = vec.y / parent.y;
		mLocalScale.z = vec.z / parent.z;
	}
	else
		mLocalScale = vec;
	
	SetUpdateFlag();
}

void Transform::SetRotation(const Quaternion quat)
{
	if (mParent)
		mLocalRotation = XMQuaternionMultiply(quat, XMQuaternionInverse(mParent->GetRotation()));
	else
		mLocalRotation = quat;

	SetUpdateFlag();
}

void Transform::SetPosition(const Vector3 vec)
{
	if (mParent)
		mLocalPosition = XMVector3TransformCoord(vec, XMMatrixInverse(nullptr, mParent->GetWorldTransform()));
	else
		mLocalPosition = vec;

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
	if (mParent)
  		mParent->RemoveChild(this);

	//Change parent and add self to new parent's children
	mParent = parent;
	parent->mChildren.push_back(this);

	SetUpdateFlag();
}

void Transform::RemoveChild(Transform * child)
{
	//Search and remove child
	for (auto iter = mChildren.begin(); iter != mChildren.end(); iter++)
		if (*iter == child) {
			mChildren.erase(iter);
			return;
		}
}

void Transform::SetUpdateFlag()
{
	mIsChanged = true;
	for (auto child : mChildren)
		child->SetUpdateFlag();
}