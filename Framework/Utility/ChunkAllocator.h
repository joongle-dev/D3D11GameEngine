#pragma once
#include <vector>
#include <memory>

class IChunkAllocator
{
public:
	IChunkAllocator() = default;
	virtual ~IChunkAllocator() = default;

	virtual void free(void* object) = 0;
};

template <class T, size_t CHUNKSIZE = 128>
class ChunkAllocator : public IChunkAllocator
{
	using store_t = std::aligned_storage_t<sizeof(T), alignof(T)>;
	using self_t = ChunkAllocator<T, CHUNKSIZE>;

public:
	ChunkAllocator() : mNumAllocated(0), mMemory{}
	{
		for (size_t i = 0; i < CHUNKSIZE; i++)
			mHandles[i] = reinterpret_cast<T*>(&mMemory[i]);
	}
	~ChunkAllocator()
	{
		for (size_t i = 0; i < mNumAllocated; i++)
			mHandles[i]->~T();
	}

	template <typename... Args>
	T* allocate(Args&&... args)
	{
		if (mNumAllocated < CHUNKSIZE)
			return new(mHandles[mNumAllocated++]) T(std::forward<Args>(args)...);

		if (!mNextChunk)
			mNextChunk = std::make_unique<self_t>();

		return mNextChunk->allocate(std::forward<Args>(args)...);
	}

	void deallocate(T* object)
	{
		if (object >= reinterpret_cast<T*>(mMemory) && object <= reinterpret_cast<T*>(mMemory) + CHUNKSIZE)
			for (size_t i = 0; i < mNumAllocated; i++)
				if (mHandles[i] == object)
				{
					object->~T();
					std::swap(mHandles[i], mHandles[--mNumAllocated]);
					return;
				}
		if (mNextChunk)
			mNextChunk->deallocate(object);
	}

	void free(void* object) override
	{
		deallocate(static_cast<T*>(object));
	}

private:
	std::unique_ptr<self_t> mNextChunk;
	size_t  mNumAllocated;
	T*      mHandles[CHUNKSIZE];
	store_t mMemory[CHUNKSIZE];

public:
	class iterator
	{
	public:
		iterator() : mChunk(nullptr), mIndex(0) {}
		iterator(self_t* chunk) : mChunk(chunk), mIndex(0)
		{
			if (mChunk->mNumAllocated <= 0 && !mChunk->mNextChunk)
				mChunk = nullptr;
		}
		~iterator() = default;

		inline iterator& operator++(int)
		{
			mIndex++;
			if (mIndex >= mChunk->numAllocated) {
				mIndex = 0;
				if (mChunk->mNextChunk)
					mChunk = mChunk->mNextChunk.get();
				else
					mChunk = nullptr;
			}
			return *this;
		}
		inline T& operator*() { return mChunk->mHandles[mIndex]; }
		inline T* operator->() { return mChunk->mHandles[mIndex]; }
		inline bool operator==(const iterator& other) { return (mIndex == other.mIndex) && (mChunk == other.mChunk); }
		inline bool operator!=(const iterator& other) { return (mIndex != other.mIndex) || (mChunk != other.mChunk); }
		operator T*() const { return mChunk->mHandles[mIndex]; }

	private:
		size_t mIndex;
		self_t* mChunk;
	};
	iterator begin() { return iterator(this); }
	iterator end() { return iterator(); }
};

/*
template <class T, size_t CHUNKSIZE = 128>
class ChunkAllocator : public IChunkAllocator
{
	//Memory chunk=========================================================
	struct Chunk
	{
		using store_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

	public:
		Chunk() : numAllocated(0), memory{}
		{
			for (size_t i = 0; i < CHUNKSIZE; i++)
				handles[i] = reinterpret_cast<T*>(&memory[i]);
		}
		~Chunk()
		{
			for (size_t i = 0; i < numAllocated; i++)
				handles[i]->~T();
		}
		T* begin() { return reinterpret_cast<T*>(memory); }

	public:
		size_t numAllocated;
		T*     handles[CHUNKSIZE];
		store_t memory[CHUNKSIZE];
	};
	using Chunks = std::vector<std::unique_ptr<Chunk>>;

public:
	//Iterator for objects allocated in memory chunks======================
	class iterator
	{
	public:
		iterator(typename Chunks::iterator begin, typename Chunks::iterator end) :
			mChunk(begin), mIndex(0) 
		{
			if ((begin != end) && (mIndex >= (*mChunk)->numAllocated))
				mChunk++;
		}
		~iterator() = default;

		inline iterator& operator++(int) 
		{
			//Increment handle index
			mIndex++;
			//If handle index reached end of chunk, go to the start of next chunk
			if (mIndex >= (*mChunk)->numAllocated) {
				mChunk++;
				mIndex = 0;
			}
			return *this;
		}
		inline T& operator*() { return (*mChunk)->handles[mIndex]; }
		inline T* operator->() { return (*mChunk)->handles[mIndex]; }
		inline bool operator==(const iterator& other) { return (mIndex == other.mIndex) && (mChunk == other.mChunk); }
		inline bool operator!=(const iterator& other) { return (mIndex != other.mIndex) || (mChunk != other.mChunk); }
		operator T*() const { return (*mChunk)->handles[mIndex]; }

	private:
		size_t mIndex;
		typename Chunks::iterator mChunk;
	};
	iterator begin() { return iterator(mChunks.begin(), mChunks.end()); }
	iterator end() { return iterator(mChunks.end(), mChunks.end()); }

public:
	//Chunk allocator=====================================================
	ChunkAllocator()
	{
		//Add initial chunk
		mChunks.emplace_back(std::make_unique<Chunk>());
	}
	~ChunkAllocator() = default;

	template <typename... Args>
	T* allocate(Args&&... args)
	{
		//If chunk with available space is found, allocate and return object
		for (auto& chunk : mChunks)
		{
			if (chunk->numAllocated >= CHUNKSIZE) continue;
			return new(chunk->handles[chunk->numAllocated++]) T(std::forward<Args>(args)...);
		}
		
		//If no avaiable chunk is found, add new chunk, allocate and return object
		auto chunk = mChunks.emplace_back(std::make_unique<Chunk>()).get();
		return new(chunk->handles[chunk->numAllocated++]) T(std::forward<Args>(args)...);
	}

	void free(T* object)
	{
		object->~T();
		//Iterate chunks until chunk that holds the object is found
		for (auto& chunk : mChunks)
			if (object >= chunk->begin() && object <= chunk->begin() + CHUNKSIZE)
				//Iterate objects in the chunk until matching object is found
				for (size_t i = 0; i < chunk->numAllocated; i++)
					if (chunk->handles[i] == object)
					{
						//Free object address
						std::swap(chunk->handles[i], chunk->handles[--chunk->numAllocated]);
						return;
					}
	}

	void free(void* object) override
	{
		free(static_cast<T*>(object));
	}

private:
	Chunks mChunks;
};
*/