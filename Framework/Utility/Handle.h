#pragma once
#include <vector>

namespace Util
{
	struct Handle
	{
		uint32_t index;
		uint32_t version;
		Handle() : index(0), version(0) {}
		Handle(uint32_t index, uint32_t version) : index(index), version(version) {}
	};

	template <class T, size_t limit = 4096, size_t growth = 512>
	class HandleTable
	{
		using TableEntry =  std::pair<uint32_t, T*>;

	private:
		std::vector<TableEntry> m_Table;

	public:
		HandleTable()
		{
			ExpandTable();
		}

		~HandleTable() = default;

		//Expand instance array
		void ExpandTable()
		{
			size_t oldSize = m_Table.size();
			assert(oldSize <= limit, "Instance limit reached.");
			size_t newSize = oldSize + growth > limit ? oldSize + growth : limit;
			m_Table.resize(newSize);
			ZeroMemory(&m_Table[oldSize], sizeof(TableEntry) * growth);
		}

		//Allocate instance and return handle to it
		Handle AllocateHandle(T* object)
		{
			size_t i;
			//Search for unused index
			for (i = 0; i < m_Table.size(); i++)
			{
				if (m_Table[i].second == nullptr)
				{
					m_Table[i] = TableEntry(m_Table[i].first + 1, object);
					return Handle(i, m_Table[i].first);
				}
			}
			//Expand table if unused index not found
			ExpandTable();
			m_Table[i] = TableEntry(m_Table[i].first + 1, object);
			return Handle(i, m_Table[i].first);
		}

		Handle AllocateHandle(T* object, uint32_t index)
		{
			while (index >= m_Table.size())
				ExpandTable();
			m_Table[index] = TableEntry(m_Table[index].first + 1, object);
			return Handle(index, m_Table[index].first);
		}

		//Release slot in table
		void ReleaseHandle(Handle handle)
		{
			assert(handle.index < m_Table.size(), "Invalid handle");
			if (m_Table[handle.index].first == handle.version)
				m_Table[handle.index].second = nullptr;
		}

		//Check if given handle is expired
		inline bool IsExpired(Handle handle)
		{
			assert(handle.index < m_Table.size(), "Invalid handle");
			//Return version expired OR instance expired
			return (m_Table[handle.index].first != handle.version) || (m_Table[handle.index].second == nullptr);
		}

		//Return handle of given index
		inline Handle operator[](uint32_t index)
		{
			assert(index < m_Table.size(), "Invalid handle");
			return Handle(index, m_Table[index].first);
		}

		//Return instance of given handle
		inline T* operator[](Handle handle)
		{
			assert(handle.index < m_Table.size(), "Invalid handle");
			if (m_Table[handle.index].first == handle.version)
				return m_Table[handle.index].second;
		}
	};
}