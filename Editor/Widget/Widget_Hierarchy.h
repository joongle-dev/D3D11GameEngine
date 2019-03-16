#pragma once
#include "IWidget.h"

class Widget_Hierarchy final : public IWidget
{
public:
	Widget_Hierarchy(class Context* context);
	~Widget_Hierarchy() = delete;

	void Render() override;

	void AddScene(class Scene* scene);
	void AddObject(class Scene* scene, class Transform* transform);

	void ContextMenu(class Scene* scene, class Transform* parent);
	void CreateObject(class Scene* scene, class Transform* parent, std::string name = "Empty Object");
};