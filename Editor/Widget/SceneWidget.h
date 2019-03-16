#pragma once
#include "IWidget.h"

class SceneWidget final : public IWidget
{
public:
	SceneWidget(class Context* context);
	~SceneWidget() = default;

	void Render() override;

	void CameraControl();

private:
	std::unique_ptr<class RenderTarget> m_rendertarget;
	std::unique_ptr<class GameObject>   m_camera;
};