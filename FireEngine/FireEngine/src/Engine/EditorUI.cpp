#include "EditorUI.h"

EditorUI::EditorUI(bool enabled)
    : m_enabled(enabled)
{
}

bool EditorUI::IsEnabled() const
{
    return m_enabled;
}

#ifdef FIREENGINE_ENABLE_IMGUI
void EditorUI::DrawBanner(const std::string& title, const std::string& subtitle) const
{
    if (!m_enabled)
    {
        return;
    }

    ImGui::SeparatorText(title.c_str());
    ImGui::TextUnformatted(subtitle.c_str());
}

void EditorUI::DrawSection(const std::string& title, const DrawCallback& drawContent) const
{
    if (!m_enabled)
    {
        return;
    }

    if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        drawContent();
    }
}

bool EditorUI::DrawToggle(const char* label, bool* value) const
{
    return m_enabled ? ImGui::Checkbox(label, value) : false;
}

bool EditorUI::DrawDragVec3(const std::string& label, float* values, float speed) const
{
    return m_enabled ? ImGui::DragFloat3(label.c_str(), values, speed) : false;
}
#endif
