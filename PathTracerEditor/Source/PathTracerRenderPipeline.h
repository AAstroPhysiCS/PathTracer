#pragma once

#include "OpenGL/OpenGLRenderer.h"
#include "OpenGL/OpenGLComputeShader.h"
#include "OpenGL/OpenGLGraphicsShader.h"

#include "Scene/Scene.h"
#include "Scene/Light.h"

/*
	Normally, i would implement a base class "RenderPipeline"
	for different render pipelines (like rasterizer)... since this project is only for path tracing
	I will just leave this as is and not generalize it further
*/

struct PathTracerRenderPipelineCreateInfo {
	std::shared_ptr<PathTracer::OpenGLRenderer> Renderer;
	std::shared_ptr<PathTracer::Window> Window;
};

class PathTracer::Texture;

class PathTracerRenderPipeline final {
public:
	PathTracerRenderPipeline(const PathTracerRenderPipelineCreateInfo& createInfo);
	~PathTracerRenderPipeline() = default;

	void BeginFrame(double deltaTime);
	void RenderFrame();
	const std::shared_ptr<PathTracer::Texture>& EndFrame();
	void Shutdown();

	inline uint32_t& GetFrameCounter() { return m_FrameCounter; }
	inline PathTracer::DirectionalLight& GetDirectionalLight() { return m_DirectionalLight; }

	inline PathTracer::Scene& GetScene() { return m_Scene; }
private:
	std::shared_ptr<PathTracer::OpenGLComputeShader> m_ComputeShader;

	std::shared_ptr<PathTracer::OpenGLRenderer> m_Renderer;
	std::shared_ptr<PathTracer::Window> m_Window;

	PathTracer::Scene m_Scene;

	std::shared_ptr<PathTracer::Texture> m_FinalTexture;

	std::shared_ptr<PathTracer::UniformBuffer> m_CameraBuffer;
	std::shared_ptr<PathTracer::UniformBuffer> m_SettingsBuffer;

	std::shared_ptr<PathTracer::StorageBuffer> m_TriangleBuffer;
	std::shared_ptr<PathTracer::StorageBuffer> m_MaterialBuffer;
	std::shared_ptr<PathTracer::StorageBuffer> m_BVHBuffer;
	std::shared_ptr<PathTracer::StorageBuffer> m_EntityBuffer;

	std::shared_ptr<PathTracer::UniformBuffer> m_DirectionalLightBuffer;
	PathTracer::DirectionalLight m_DirectionalLight{};

	uint32_t m_FrameCounter = 0;
};

