#pragma once
#include "IWidget.h"

class GameWidget final : public IWidget
{
public:
	GameWidget(class Context* context);
	~GameWidget() = default;

	void Render() override;

private:
	class RenderTarget* m_rendertarget;
};