#pragma once

namespace Util
{
	typedef uint16_t TypeID;

	template <class Base>
	class FamilyTypeID
	{
	private:
		static inline unsigned int s_count = 0;

	public:
		template <class T>
		static constexpr TypeID GetID()
		{
			static const TypeID ID = s_count++;
			return ID;
		}
		static unsigned int GetCount()
		{
			return s_count;
		}
	};
}