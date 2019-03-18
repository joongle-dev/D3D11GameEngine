#pragma once
#include "json.hpp"
#include "SimpleMath.h"

#define PI DirectX::XM_PI

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