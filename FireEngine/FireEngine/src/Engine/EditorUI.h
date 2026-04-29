#pragma once

#ifdef FIREENGINE_ENABLE_IMGUI
#include <imgui.h>
#endif

#include <functional>
#include <string>

class EditorUI
{
public:
#ifdef FIREENGINE_ENABLE_IMGUI
    using DrawCallback = std::function<void()>;
#endif

    explicit EditorUI(bool enabled);

    bool IsEnabled() const;
#ifdef FIREENGINE_ENABLE_IMGUI
    void DrawBanner(const std::string& title, const std::string& subtitle) const;
    void DrawSection(const std::string& title, const DrawCallback& drawContent) const;
    bool DrawToggle(const char* label, bool* value) const;
    bool DrawDragVec3(const std::string& label, float* values, float speed = 0.05f) const;
#endif

private:
    bool m_enabled = false;
};
