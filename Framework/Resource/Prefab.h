#pragma once
#include "Resource.h"

class Prefab final : public Resource<Prefab>
{
public:
	Prefab(class Context* context);
	~Prefab() = default;

	void LoadFromFile(const std::string& path) override;
	void SaveToFile(const std::string& path) override;

private:
};