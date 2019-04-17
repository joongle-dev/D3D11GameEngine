#pragma once

class EditorHelper final
{
public:
	static class GameObject* sSelected;
};

template <class T>
void ImGuiDragSource(const char* label, T& source)
{
	if (ImGui::BeginDragDropSource(0))
	{
		ImGui::SetDragDropPayload(label, &source, sizeof(T));
		ImGui::EndDragDropSource();
	}
}

template <class T>
T ImGuiDropTarget(const char* label)
{
	T source = {};
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(label))
			source = *static_cast<T*>(payload->Data);
		ImGui::EndDragDropTarget();
	}
	return source;
}