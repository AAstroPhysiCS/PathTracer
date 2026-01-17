#pragma once
#include "Application.h"
#include "PathTracerRenderPipeline.h"
#include "Renderer.h"
#include "Core/Window.h"
#include <memory>
#include <format>

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationCreateInfo& createInfo);
	virtual ~EditorApplication() = default;
	
	void Initialize() override;
	void Run() override;
	int32_t Shutdown() override;
private:
	std::shared_ptr<PathTracer::Renderer> m_Renderer;
	std::shared_ptr<PathTracer::Window> m_Window;

	std::shared_ptr<PathTracerRenderPipeline> m_RenderPipeline;
};