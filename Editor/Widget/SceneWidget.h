#pragma once
#include "IWidget.h"

class SceneWidget final : public IWidget
{
public:
	SceneWidget(class Context* context);
	~SceneWidget() = default;

	void Render() override;

	void CameraControl();
	void ShowFrame();
	void ShowGizmo();

private:
	std::unique_ptr<class RenderTarget> m_rendertarget;
	std::unique_ptr<class GameObject>   m_camera;

	ImVec2 frameSize;
	ImVec2 framePos;

	Vector2 m_angles;
};