#pragma once
#include <vector>
#include <fstream>

class FileStreamRead final
{
public:
	FileStreamRead() = default;
	~FileStreamRead() = default;

	void Open(const std::string& path)
	{
		stream.open(path, std::ios::in | std::ios::binary);
		assert(!stream.fail());
	}
	void Open(const std::wstring& path)
	{
		stream.open(path, std::ios::in | std::ios::binary);
		assert(!stream.fail());
	}
	void Close()
	{
		stream.close();
	}

	void Read(void* ptr, size_t size)
	{
		stream.read(reinterpret_cast<char*>(ptr), size);
	}
	void Read(std::string& value)
	{
		uint32_t length;
		Read(length);
		value.clear();
		value.resize(length);
		stream.read(value.data(), length);
	}
	template <typename T>
	void Read(std::vector<T>& value)
	{
		uint32_t length;
		Read(length);
		value.clear();
		value.resize(length);
		stream.read(reinterpret_cast<char*>(value.data()), sizeof(T) * length);
	}
	template <typename T>
	void Read(T& value)
	{
		stream.read(reinterpret_cast<char*>(&value), sizeof(T));
	}

private:
	std::ifstream stream;
};

class FileStreamWrite final
{
public:
	FileStreamWrite() = default;
	~FileStreamWrite() = default;

	void Open(const std::string& path)
	{
		stream.open(path, std::ios::out | std::ios::binary);
		assert(!stream.fail());
	}
	void Open(const std::wstring& path)
	{
		stream.open(path, std::ios::out | std::ios::binary);
		assert(!stream.fail());
	}
	void Close()
	{
		stream.close();
	}

	void Write(void* ptr, size_t size)
	{
		stream.write(reinterpret_cast<const char*>(ptr), size);
	}
	void Write(std::string& value)
	{
		uint32_t length = value.length();
		Write(length);
		stream.write(value.data(), length);
	}
	template <typename T>
	void Write(std::vector<T>& value)
	{
		uint32_t length = value.size();
		Write(length);
		stream.write(reinterpret_cast<const char*>(value.data()), sizeof(T) * length);
	}
	template <typename T>
	void Write(T& value)
	{
		stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

private:
	std::ofstream stream;
};