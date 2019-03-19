#pragma once
#include "json.hpp"
#include "SimpleMath.h"

static constexpr float PI		= 3.14159265359f;
static constexpr float PI_2		= 6.28318530718f;
static constexpr float PI_DIV_2 = 1.57079632679f;
static constexpr float PI_DIV_4 = 0.78539816339f;
static constexpr float TO_DEG	= 180.0f / PI;
static constexpr float TO_RAD	= PI / 180.0f;

using Vector2	 = DirectX::SimpleMath::Vector2;
using Vector3	 = DirectX::SimpleMath::Vector3;
using Vector4	 = DirectX::SimpleMath::Vector4;
using Quaternion = DirectX::SimpleMath::Quaternion;
using Matrix	 = DirectX::SimpleMath::Matrix;
using Color		 = DirectX::SimpleMath::Color;

namespace nlohmann {

	template <>
	struct adl_serializer<DirectX::SimpleMath::Vector2> {
		static void to_json(nlohmann::json& j, const Vector2& p) {
			j = nlohmann::json{ p.x, p.y };
		}
		static void from_json(const nlohmann::json& j, Vector2& p) {
			j.at(0).get_to(p.x);
			j.at(1).get_to(p.y);
		}
	};

	template <>
	struct adl_serializer<DirectX::SimpleMath::Vector3> {
		static void to_json(nlohmann::json& j, const Vector3& p) {
			j = nlohmann::json{ p.x, p.y, p.z };
		}
		static void from_json(const nlohmann::json& j, Vector3& p) {
			j.at(0).get_to(p.x);
			j.at(1).get_to(p.y);
			j.at(2).get_to(p.z);
		}
	};

	template <>
	struct adl_serializer<DirectX::SimpleMath::Vector4> {
		static void to_json(nlohmann::json& j, const Vector4& p) {
			j = nlohmann::json{ p.x, p.y, p.z, p.w };
		}
		static void from_json(const nlohmann::json& j, Vector4& p) {
			j.at(0).get_to(p.x);
			j.at(1).get_to(p.y);
			j.at(2).get_to(p.z);
			j.at(3).get_to(p.w);
		}
	};

	template <>
	struct adl_serializer<DirectX::SimpleMath::Quaternion> {
		static void to_json(nlohmann::json& j, const Quaternion& p) {
			j = nlohmann::json{ p.x, p.y, p.z, p.w };
		}
		static void from_json(const nlohmann::json& j, Quaternion& p) {
			j.at(0).get_to(p.x);
			j.at(1).get_to(p.y);
			j.at(2).get_to(p.z);
			j.at(3).get_to(p.w);
		}
	};
}