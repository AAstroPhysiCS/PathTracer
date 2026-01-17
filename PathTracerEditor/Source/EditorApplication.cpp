#include "EditorApplication.h"

#include "OpenGL/OpenGLRenderer.h"

#include "GLFW/glfw3.h"

#include "UI/RendererSettingsPanel.h"
#include "UI/MainDockSpacePanel.h"
#include "UI/ViewportPanel.h"
#include "UI/ScenePanel.h"

EditorApplication::EditorApplication(const ApplicationCreateInfo& createInfo)
	: Application(createInfo) {
}

void EditorApplication::Initialize() {
	m_Window = std::make_shared<PathTracer::Win32Window>(PathTracer::WindowCreateInfo{
		.Title = GetTitle(),
		.Width = GetWidth(),
		.Height = GetHeight(),
		.TargetAPI = GetTargetAPI()
	});

	m_Window->Initialize();

	switch (GetTargetAPI()) {
		case RendererAPI::OpenGL: {
			auto renderer = std::make_shared<PathTracer::OpenGLRenderer>(PathTracer::RendererCreateInfo{
				.Width = GetWidth(),
				.Height = GetHeight(),
				.TargetAPI = GetTargetAPI(),
				.Window = m_Window
			});

			renderer->Initialize();

			m_RenderPipeline = std::make_shared<PathTracerRenderPipeline>(PathTracerRenderPipelineCreateInfo{
				.Renderer = renderer,
				.Window = m_Window
			});

			m_Renderer = renderer;
			break;
		}
		default:
			PT_ASSERT(false, "EditorApplication: Initialize failed, no supported renderer api found!");
	}

	m_Window->AddUIPanel<MainDockSpacePanel>();
	m_Window->AddUIPanel<ViewportPanel>(m_Window, m_RenderPipeline->GetScene());
	m_Window->AddUIPanel<ScenePanel>(m_RenderPipeline->GetFrameCounter(), m_RenderPipeline->GetScene(), m_Renderer->GetRenderDevice());
	m_Window->AddUIPanel<RendererSettingsPanel>(m_RenderPipeline->GetFrameCounter(), m_RenderPipeline->GetDirectionalLight());
}

void EditorApplication::Run() {
	double deltaBefore = 0.0;
	double frameBefore = 0.0f;
	int32_t frameCount = 0;

	while (!m_Window->ShouldClose()) {
		double timeNow = glfwGetTime();
		double deltaTime = timeNow - deltaBefore;
		frameCount++;

		if (timeNow - frameBefore >= 1.0) {
			m_Window->SetTitle(std::format("PathTracer Editor FPS: {0}", frameCount));

			frameCount = 0;
			frameBefore = timeNow;
		}

		m_Window->GetUIPanel<RendererSettingsPanel>()->GetStats().FrameTimeMS = deltaTime * 1000.0f;

		m_Window->Begin(deltaTime);
		m_RenderPipeline->BeginFrame(deltaTime);

		m_Window->RenderUI();
		m_RenderPipeline->RenderFrame();

		const auto& finalTexture = m_RenderPipeline->EndFrame();
		m_Window->GetUIPanel<ViewportPanel>()->SetViewportTexture(finalTexture);
		m_Window->End();

		deltaBefore = timeNow;
	}
}

int32_t EditorApplication::Shutdown() {
	m_RenderPipeline->Shutdown();
	return 0;
}