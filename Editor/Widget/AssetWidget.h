#pragma once
#include "IWidget.h"

class AssetWidget final : public IWidget
{
public:
	AssetWidget(class Context* context);
	~AssetWidget() = delete;

	void Render() override;

private:
	class ResourceManager* mResourceManager;
};