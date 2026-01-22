#include "PathTracerRenderPipeline.h"

#include "Texture.h"

#include "Buffer/UniformBuffer.h"
#include "Buffer/StorageBuffer.h"

#include "UI/RendererSettingsPanel.h"

#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "Core/Event.h"
#include "DrawCommands.h"

#include "GPUData.h"

PathTracerRenderPipeline::PathTracerRenderPipeline(const PathTracerRenderPipelineCreateInfo& createInfo) 
	: m_Renderer(createInfo.Renderer), m_Window(createInfo.Window) {

	const auto& renderDevice = m_Renderer->GetRenderDevice();
	m_FinalTexture = renderDevice->CreateTexture({
		.Width = 1280,
		.Height = 720,
		.Format = PathTracer::TextureFormat::RGBA32_Float
	});

	m_CameraBuffer = renderDevice->CreateUniformBuffer("u_Camera", sizeof(PathTracer::CameraBufferGPUData));
	m_SettingsBuffer = renderDevice->CreateUniformBuffer("u_Settings", sizeof(PathTracer::SettingsBufferGPUData));
	m_DirectionalLightBuffer = renderDevice->CreateUniformBuffer("u_DirectionalLight", sizeof(PathTracer::DirectionalLight));

	//2 GB
	m_TriangleBuffer = renderDevice->CreateStorageBuffer("s_Triangles", 2048 * 1024 * 1024);
	m_MaterialBuffer = renderDevice->CreateStorageBuffer("s_Materials", 256 * 1024 * 1024);
	m_BVHBuffer = renderDevice->CreateStorageBuffer("s_BVHNodes", 512 * 1024 * 1024);
	m_EntityBuffer = renderDevice->CreateStorageBuffer("s_Entities", 1024);

	m_ComputeShader = renderDevice->CreateComputeShader("Assets/Shaders/pathtracer.comp");
}

void PathTracerRenderPipeline::BeginFrame(double deltaTime) {
	PathTracer::EventSystem::AddEventListener<PathTracer::FramebufferResizeEvent>([&](const PathTracer::FramebufferResizeEvent& evt) {
		m_FinalTexture->Resize(evt.GetWidth(), evt.GetHeight());
	});

	static uint32_t oldTriangleCount = 0;

	m_Scene.Begin(m_Window, deltaTime);
	m_Renderer->BeginFrame();

	const auto& camera = m_Scene.GetCamera();
	const auto& [right, up, dir] = camera.GetCameraVectors();

	PathTracer::CameraBufferGPUData cameraBuffer {
		.Position = camera.GetPosition(),
		.Direction = dir,
		.Right = right,
		.Up = up,
		.Fov = camera.GetFOV(),
	};

	const auto& panel = m_Window->GetUIPanel<RendererSettingsPanel>();
	const auto& settings = panel->GetSettings();
	auto& stats = panel->GetStats();

	PathTracer::SettingsBufferGPUData settingsBuffer {
		.FrameCounter = (camera.IsMouseMoved() || camera.IsCameraMoved()) ? m_FrameCounter = 0 : m_FrameCounter++,
		.TotalTriangleCount = static_cast<uint32_t>(m_Scene.GetTotalTriangleCount()),
		.MaxBounces = static_cast<uint32_t>(settings.MaxBounces),
		.SamplesPerFrame = static_cast<uint32_t>(settings.SamplesPerFrame),
		.MaxSamples = static_cast<uint32_t>(settings.MaxSamples),
		.Accumulate = settings.Accumulate,
		.UseMIS = settings.UseMIS,
		.EnableDenoiser = settings.EnableDenoiser,
		.ShowBVH = settings.ShowBVH,
		.ShowRays = settings.ShowRays,
		.Exposure = settings.Exposure,
		.Gamma = settings.Gamma,
	};

	stats.SamplesAccumulated = glm::min(m_FrameCounter, settingsBuffer.MaxSamples);

	std::vector<PathTracer::EntityGPUData> entityData;
	entityData.resize(m_Scene.GetAllEntities().size()); 
	for (size_t i = 0; auto& entity : m_Scene.GetAllEntities()) {
		PathTracer::EntityGPUData entityBuffer{};
		entityBuffer.Transform = entity->CalculateModelMatrix();	
		entityBuffer.InverseTransposeTransform = glm::transpose(glm::inverse(entityBuffer.Transform));
		entityData[i++] = entityBuffer;
	}

	m_EntityBuffer->SetData(entityData.data(), entityData.size() * sizeof(PathTracer::EntityGPUData));

	if (oldTriangleCount != settingsBuffer.TotalTriangleCount) {
		oldTriangleCount = settingsBuffer.TotalTriangleCount;
		m_FrameCounter = 0; 

		auto& entity = m_Scene.GetAllEntities().back();
		const auto& mesh = entity->GetMesh();

		const auto& triangleBufferGPUDatas = mesh.GetTriangleBufferGPUDatas();
		for (auto& buff : triangleBufferGPUDatas)
			m_TriangleBuffer->AppendData(&buff, sizeof(buff));

		const auto& materialBufferGPUDatas = mesh.GetMaterialBufferGPUDatas();
		for (const auto& buff : materialBufferGPUDatas)
			m_MaterialBuffer->AppendData(&buff, sizeof(buff));

		const auto& bvhGPUDatas = mesh.GetBVHGPUDatas();
		for (const auto& bvh : bvhGPUDatas)
			m_BVHBuffer->AppendData(&bvh, sizeof(bvh));
	}

	m_CameraBuffer->SetData(&cameraBuffer, sizeof(PathTracer::CameraBufferGPUData));
	m_SettingsBuffer->SetData(&settingsBuffer, sizeof(settingsBuffer));

	auto lightCopy = m_DirectionalLight;
	glm::mat4 mat = glm::eulerAngleZX(glm::radians(lightCopy.Direction.x), glm::radians(lightCopy.Direction.y));
	lightCopy.Direction = mat * glm::vec4(PathTracer::FORWARD_VECTOR, 1.0f);

	m_DirectionalLightBuffer->SetData(&lightCopy, sizeof(lightCopy));
}

void PathTracerRenderPipeline::RenderFrame() {
	PathTracer::ComputeDrawCommand cmd {
		.Shader = m_ComputeShader,
		.Images = {
			{ .Name = "u_AccumImage", .Binding = 0, .Access = PathTracer::TextureAccess::WriteOnly, .Texture = m_FinalTexture },
		},
		.Uniforms = {
			{ .Name = "u_Camera", .Binding = 0, .Handle = m_CameraBuffer },
			{ .Name = "u_Settings", .Binding = 1, .Handle = m_SettingsBuffer },
			{ .Name = "u_DirectionalLight", .Binding = 2, .Handle = m_DirectionalLightBuffer },
		},
		.SSBOs = {
			{ .Name = "s_Triangles", .Binding = 0, .Handle = m_TriangleBuffer },
			{ .Name = "s_Materials", .Binding = 1, .Handle = m_MaterialBuffer },
			{ .Name = "s_Entities", .Binding = 2, .Handle = m_EntityBuffer },
			{ .Name = "s_BVHNodes", .Binding = 3, .Handle = m_BVHBuffer },
		},
		.NumWorkGroup = {
			.X = m_FinalTexture->GetWidth() / 8,
			.Y = m_FinalTexture->GetHeight() / 8,
			.Z = 1
		}
	};

	m_Renderer->DispatchComputeDrawCommand(cmd);
	m_Renderer->SetMemoryBarrier(PathTracer::MemoryBarrier::ShaderImage);
	//m_Renderer->RenderToSwapChain(m_FinalTexture);
}

const std::shared_ptr<PathTracer::Texture>& PathTracerRenderPipeline::EndFrame() {
	m_Renderer->EndFrame();
	m_Scene.End();

	return m_FinalTexture;
}

void PathTracerRenderPipeline::Shutdown() {
	m_Renderer->Shutdown();
	m_Window->Shutdown();
}
