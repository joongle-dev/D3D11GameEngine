#pragma once
#include "IWidget.h"

class InspectorWidget final : public IWidget
{
public:
	InspectorWidget(class Context* context);
	~InspectorWidget() = delete;

	void Render() override;

private:
	void ShowTransform();
	void ShowCamera();
	void ShowMeshRender();
	void ShowSkinnedMeshRender();
	void ShowAnimator();
	void ShowLight();

	void ShowMaterial(class Material* pMaterial);
	void ShowMesh(class Mesh* pMesh);
};