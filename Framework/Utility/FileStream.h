#pragma once
#include <vector>
#include <fstream>

class FileStreamRead final
{
public:
	FileStreamRead() : bOpen(false) {}
	FileStreamRead(const std::string& path)
	{
		Open(path);
	}
	~FileStreamRead()
	{
		Close();
	}

	void Open(const std::string& path)
	{
		stream.open(path, std::ios::in | std::ios::binary);
		assert((!stream.fail(), "Failed to open file"));
		bOpen = true;
	}
	void Open(const std::wstring& path)
	{
		stream.open(path, std::ios::in | std::ios::binary);
		assert((!stream.fail(), "Failed to open file"));
		bOpen = true;
	}
	void Close()
	{
		if (bOpen)
		{
			stream.close();
			bOpen = false;
		}
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
	template <typename T>
	T Read()
	{
		T value;
		Read(value);
		return value;
	}

private:
	std::ifstream stream;
	bool bOpen;
};

class FileStreamWrite final
{
public:
	FileStreamWrite() : bOpen(false) {}
	FileStreamWrite(const std::string& path)
	{
		Open(path);
	}
	~FileStreamWrite()
	{
		Close();
	}

	void Open(const std::string& path)
	{
		stream.open(path, std::ios::out | std::ios::binary);
		assert((!stream.fail(), "Failed to create file"));
		bOpen = true;
	}
	void Open(const std::wstring& path)
	{
		stream.open(path, std::ios::out | std::ios::binary);
		assert((!stream.fail(), "Failed to create file"));
		bOpen = true;
	}
	void Close()
	{
		if (bOpen)
		{
			stream.close();
			bOpen = false;
		}
	}

	void Write(void* ptr, size_t size)
	{
		stream.write(reinterpret_cast<const char*>(ptr), size);
	}
	void Write(const std::string& value)
	{
		uint32_t length = value.length();
		Write(length);
		stream.write(value.data(), length);
	}
	template <typename T>
	void Write(const std::vector<T>& value)
	{
		uint32_t length = value.size();
		Write(length);
		stream.write(reinterpret_cast<const char*>(value.data()), sizeof(T) * length);
	}
	template <typename T>
	void Write(const T& value)
	{
		stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

private:
	std::ofstream stream;
	bool bOpen;
};