#pragma once
#include "imgui.h"

#include "UIPanel.h"

class MainDockSpacePanel : public PathTracer::UIPanel {
public:
    MainDockSpacePanel() = default;

    void Begin() const override;
    void End() const override;
private:
    bool m_DockspaceOpen = true;
    ImGuiDockNodeFlags m_DockspaceFlags = ImGuiDockNodeFlags_None;
};
