#pragma once
#include "json.hpp"

template <class Class>
class Serializable
{
	using json = nlohmann::json;

	template <typename T>
	using member_ptr_t = T Class::*;

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

	enum class FieldType : unsigned int
	{
		member,
		const_reference,
		nonconst_reference,
		const_value,
		nonconst_value
	};

	class IField
	{
	public:
		virtual ~IField() = default;

		virtual json toJson(Class& object) = 0;
		virtual void fromJson(Class& object, const json& j) = 0;
	};

	template <typename T>
	class Field : public IField
	{
	private:
		FieldType fieldType;
		member_ptr_t<T> memPtr;
		const_ref_getter_ptr_t<T> refGetter;
		const_ref_setter_ptr_t<T> refSetter;
		const_val_getter_ptr_t<T> valGetter;
		const_val_setter_ptr_t<T> valSetter;

	public:
		Field(const_val_getter_ptr_t<T> getter, const_val_setter_ptr_t<T> setter) :
			valGetter(getter), valSetter(setter), fieldType(FieldType::const_value) {}

		json toJson(Class& object) override
		{
			switch (fieldType)
			{
				case FieldType::member:
					return json(object.*memPtr);
				case FieldType::const_value:
					return json((object.*valGetter)());
			}
			return json();
		}

		void fromJson(Class& object, const json& j) override
		{
			switch (fieldType)
			{
				case FieldType::member:
					object.*memPtr = j;
				case FieldType::const_value:
					(object.*valSetter)(j);
			};
		}
	};

	using SerializeField = std::pair<std::string, std::unique_ptr<IField>>;
	using SerializeFields = std::vector<SerializeField>;

public:
	template <typename T>
	static void RegisterSerializeField(const char* name, const_val_getter_ptr_t<T> getter, const_val_setter_ptr_t<T> setter)
	{
		s_serializefields.emplace_back(name, std::make_unique<Field<T>>(getter, setter));
	}

	virtual void RegisterSerializeFields() {}

private:
	static SerializeFields s_serializefields;
};

template <class Class>
typename Serializable<Class>::SerializeFields Serializable<Class>::s_serializefields;