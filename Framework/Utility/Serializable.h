#pragma once
#include <vector>
#include "json.hpp"

template <class Class>
class Serializable
{
public:
	using Json = nlohmann::json;

	template <typename T>
	using member_ptr = T Class::*;
	template <typename T>
	using const_ref_getter_ptr_t = const T& (Class::*)() const;
	template <typename T>
	using const_ref_setter_ptr_t = void (Class::*)(const T&);
	template <typename T>
	using ref_getter_ptr_t = const T& (Class::*)();
	template <typename T>
	using ref_setter_ptr_t = void (Class::*)(T&);
	template <typename T>
	using const_val_getter_ptr_t = const T(Class::*)();
	template <typename T>
	using const_val_setter_ptr_t = void (Class::*)(const T);
	template <typename T>
	using val_getter_ptr_t = T(Class::*)();
	template <typename T>
	using val_setter_ptr_t = void (Class::*)(T);

	class IField
	{
	public:
		virtual ~IField() = default;

		virtual Json ToJson(Class& object) = 0;
		virtual void FromJson(Class& object, const Json& j) = 0;
	};

	template <typename ...PARAM>
	class Field : public IField { };

	template <typename GET, typename SET>
	class Field<GET, SET>
	{
	private:
		GET mGetter;
		SET mSetter;

	public:
		Field(GET getter, SET setter) : mGetter(getter), mSetter(setter) {}

		Json ToJson(Class& object) override
		{
			return Json((object.*mGetter)());
		}
		void FromJson(Class& object, const Json& j) override
		{
			(object.*mSetter)(j);
		}
	};

	template <typename T>
	class Field<member_ptr<T>>
	{
	private:
		member_ptr<T> mMember;

	public:
		Field(member_ptr<T> member) : mMember(member) {}

		Json ToJson(Class& object) override
		{
			return Json(object.*mMember);
		}
		void FromJson(Class& object, const Json& j) override
		{
			object.*mMember = j;
		}
	};

	using FieldPtr = std::pair<std::string, IField*>;

public:
	template <typename T>
	static void SerializeField(const std::string& name, member_ptr<T> member)
	{
		sSerializeFields.emplace_back(name, new Field(member));
	}
	template <typename T>
	static void SerializeField(const std::string& name, const_ref_getter_ptr_t<T> getter, const_ref_setter_ptr_t<T> setter)
	{
		sSerializeFields.emplace_back(name, new Field(getter, setter));
	}
	template <typename T>
	static void SerializeField(const std::string& name, ref_getter_ptr_t<T> getter, ref_setter_ptr_t<T> setter)
	{
		sSerializeFields.emplace_back(name, new Field(getter, setter));
	}
	template <typename T>
	static void SerializeField(const std::string& name, const_val_getter_ptr_t<T> getter, const_val_setter_ptr_t<T> setter)
	{
		sSerializeFields.emplace_back(name, new Field(getter, setter));
	}
	template <typename T>
	static void SerializeField(const std::string& name, val_getter_ptr_t<T> getter, val_setter_ptr_t<T> setter)
	{
		sSerializeFields.emplace_back(name, new Field(getter, setter));
	}

	static Json Serialize(Class& object)
	{
		Json serialized;
		for (auto& iter : sSerializeFields)
			serialized[iter.first] = iter.second->ToJson(object);
		return serialized;
	}
	static void Deserialize(Class& object, Json j)
	{
		for (auto& iter : sSerializeFields)
			iter.second->FromJson(object, j[iter.first]);
	}

private:
	static std::vector<FieldPtr> sSerializeFields;
};

template <class Class>
std::vector<typename Serializable<Class>::FieldPtr> Serializable<Class>::sSerializeFields;