#pragma once

#include "UIPanel.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

class PathTracer::RenderDevice;

class ScenePanel : public PathTracer::UIPanel {
public:
    ScenePanel(uint32_t& frameCounter, PathTracer::Scene& scene, const std::shared_ptr<PathTracer::RenderDevice>& device);
    virtual ~ScenePanel() = default;

    void Begin() const override;
    void End() const override;
private:
    void DrawEntityList() const;
    void DrawEntityInspector() const;
    void HandleMeshLoading() const;
private:
    mutable std::shared_ptr<PathTracer::Entity> m_SelectedEntity = nullptr;

    PathTracer::Scene& m_Scene;
    uint32_t& m_FrameCounter;

    std::shared_ptr<PathTracer::RenderDevice> m_RenderDevice;
};