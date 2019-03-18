#pragma once
#include "json.hpp"
#include "SimpleMath.h"

class Vector2 final : public DirectX::SimpleMath::Vector2
{
	using DirectX::SimpleMath::Vector2::Vector2;
};

class Vector3 final : public DirectX::SimpleMath::Vector3
{
	using DirectX::SimpleMath::Vector3::Vector3;
};

void to_json(nlohmann::json& j, const Vector3& p) {
	j = nlohmann::json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
}

void from_json(const nlohmann::json& j, Vector3& p) {
	j.at("x").get_to(p.x);
	j.at("y").get_to(p.y);
	j.at("y").get_to(p.z);
}

class Vector4 final : public DirectX::SimpleMath::Vector4
{
	using DirectX::SimpleMath::Vector4::Vector4;
};

class Quaternion final : public DirectX::SimpleMath::Quaternion
{
	using DirectX::SimpleMath::Quaternion::Quaternion;
};

class Matrix final : public DirectX::SimpleMath::Matrix
{
	using DirectX::SimpleMath::Matrix::Matrix;
};

class Color final : public DirectX::SimpleMath::Color
{
	using DirectX::SimpleMath::Color::Color;
};