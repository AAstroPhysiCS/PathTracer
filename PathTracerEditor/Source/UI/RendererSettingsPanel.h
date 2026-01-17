#pragma once

#include "UIPanel.h"
#include "RendererSettings.h"
#include "Scene/Light.h"

class RendererSettingsPanel : public PathTracer::UIPanel {
public:
    RendererSettingsPanel(uint32_t& frameCounter, PathTracer::DirectionalLight& light);
	virtual ~RendererSettingsPanel() = default;

	void Begin() const override;
	void End() const override;

    static PathTracer::RendererSettings& GetSettings();
    static PathTracer::RendererStats& GetStats();
private:
	static inline PathTracer::RendererSettings s_Settings{};
    static inline PathTracer::RendererStats s_Stats{};

    uint32_t& m_FrameCounter;
    PathTracer::DirectionalLight& m_DirLight;
};

