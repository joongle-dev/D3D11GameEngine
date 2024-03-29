#pragma once
#include "IWidget.h"

class HierarchyWidget final : public IWidget
{
public:
	HierarchyWidget(class Context* context);
	~HierarchyWidget() = delete;

	void Render() override;

private:
	void SceneNode(class Scene* scene);
	void ObjectNode(class Scene* scene, class Transform* transform);

	void DragSource(class Transform* src);
	void DropTarget(class Transform* dst);

	void ContextMenu(class Scene* scene, class Transform* parent);
};