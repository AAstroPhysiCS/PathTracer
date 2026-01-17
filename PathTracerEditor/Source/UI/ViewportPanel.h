#pragma once
#include "UIPanel.h"
#include "Core/Window.h"
#include "Scene/Scene.h"
#include "Texture.h"

class ViewportPanel : public PathTracer::UIPanel {
public:
    ViewportPanel(const std::shared_ptr<PathTracer::Window>& window, PathTracer::Scene& scene);
    virtual ~ViewportPanel() = default;

    void Begin() const override;
    void End() const override;

    void SetViewportTexture(std::shared_ptr<PathTracer::Texture> viewportTexture);
private:
    std::shared_ptr<PathTracer::Texture> m_ViewportTexture;
    std::shared_ptr<PathTracer::Window> m_Window;
    PathTracer::Scene& m_Scene;

    mutable bool m_MouseCaptured = false;
};
