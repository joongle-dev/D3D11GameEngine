#pragma once
#include "json.hpp"

namespace Serializable
{
	using json = nlohmann::json;

	template <class Class, typename T>
	using member_ptr_t = T Class::*;

	template <class Class, typename T>
	using const_ref_getter_ptr_t = const T& (Class::*)() const;

	template <class Class, typename T>
	using const_ref_setter_ptr_t = void (Class::*)(const T&);

	template <class Class, typename T>
	using ref_getter_ptr_t = const T& (Class::*)();

	template <class Class, typename T>
	using ref_setter_ptr_t = void (Class::*)(T&);

	template <class Class, typename T>
	using const_val_getter_ptr_t = T(Class::*)() const;

	template <class Class, typename T>
	using const_val_setter_ptr_t = void (Class::*)(const T);

	template <class Class, typename T>
	using val_getter_ptr_t = T(Class::*)();

	template <class Class, typename T>
	using val_setter_ptr_t = void (Class::*)(T);

	enum class FieldType : unsigned int
	{
		member,
		const_reference,
		nonconst_reference,
		const_value,
		nonconst_value
	};

	template <class Class>
	struct ISerializeField
	{
		virtual json toJson(Class& object) = 0;
		virtual void fromJson(Class& object, const json& j) = 0;
	};

	template <class Class, typename T>
	struct SerializeField : public ISerializeField<Class>
	{
		FieldType fieldType;
		member_ptr_t<Class, T> memPtr;
		const_ref_getter_ptr_t<Class, T> refGetter;
		const_ref_setter_ptr_t<Class, T> refSetter;

		SerializeField(member_ptr_t<Class, T> member) :
			memPtr(member), fieldType(FieldType::member) {}

		SerializeField(const_ref_getter_ptr_t<Class, T> getter, const_ref_setter_ptr_t<Class, T> setter) :
			refGetter(getter), refSetter(setter), fieldType(FieldType::const_reference) {}

		json toJson(const Class& object)
		{
			switch (fieldType)
			{
				case FieldType::member:
					return json(object.*memPtr);
			}
			return json();
		}

		void fromJson(const Class& object, const json& j)
		{

		}
	};
}