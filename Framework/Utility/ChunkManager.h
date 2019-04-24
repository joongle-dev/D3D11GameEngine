#pragma once
#include <vector>
#include <memory>
#include <assert.h>

template <class T>
class Handle final
{
public:
	Handle() :
		mIndex(0), mVersion(0) {}
	Handle(unsigned short version, unsigned short index) :
		mIndex(index), mVersion(version) {}
	~Handle() = default;

	Handle& operator=(const Handle& rhs)
	{
		mIndex = rhs.mIndex;
		mVersion = rhs.mVersion;
	}
	inline bool operator==(const Handle& rhs)
	{
		return (mIndex == rhs.mIndex) && (mVersion == rhs.mVersion);
	}

private:
	unsigned short mIndex;
	unsigned short mVersion;
};

template <class T, size_t CHUNK_SIZE = 128>
class Chunk final
{
	using storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;
	using chunk_t = Chunk<T, CHUNK_SIZE>;

public:
	Chunk() : mNumAllocated(0), mVersions{ 0 }
	{
		for (size_t i = 0; i < CHUNK_SIZE; i++)
			mFreeHandles[i] = i;
	}
	~Chunk() = default;

	Handle allocate()
	{
		if (mNumAllocated < CHUNK_SIZE)
			return Handle(mNumAllocated, mVersions[mNumAllocated++]);

		if (!mNextChunk)
			mNextChunk = std::make_shared<chunk_t>();

		return mNextChunk->allocate();
	}

	T* operator[](const Handle& handle)
	{
		unsigned short index = handle.mIndex % CHUNK_SIZE;
		if (mVersions[index] = handle.mVersion)
			return &mObjects[index];
		return nullptr;
	}

private:
	std::shared_ptr<chunk_t> mNextChunk;
	unsigned short mNumAllocated;
	unsigned short mFreeHandles[CHUNK_SIZE];
	unsigned short mVersions[CHUNK_SIZE];
	storage_t mObjects[CHUNK_SIZE];
};
