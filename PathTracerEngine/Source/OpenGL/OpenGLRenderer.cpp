#include "OpenGLRenderer.h"
#include "OpenGLContext.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLComputeShader.h"
#include "OpenGLGraphicsShader.h"
#include "OpenGLTexture.h"

#include "DrawCommands.h"

#include "Core/Window.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Buffer/OpenGLUniformBuffer.h"
#include "Buffer/OpenGLStorageBuffer.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

namespace PathTracer {
	
	OpenGLRenderer::OpenGLRenderer(const RendererCreateInfo& createInfo) 
		: Renderer(createInfo) {
	}
	
	void OpenGLRenderer::Initialize() {
		Renderer::Initialize();
		m_FullscreenShader = GetRenderDevice()->CreateGraphicsShader("Assets/Shaders/fullscreen.vert", "Assets/Shaders/fullscreen.frag");

		//dummy vao since we want to only utilize compute shaders and not rasterize anything
		//this is basically for, if we want to render it on to the swapchain and not on to a imgui texture
		//see OpenGLRenderer::RenderToSwapChain
		glGenVertexArrays(1, &m_FullscrenVAO);
		glBindVertexArray(m_FullscrenVAO);
	}

	void OpenGLRenderer::DispatchComputeDrawCommand(const ComputeDrawCommand& cmd) {
		const auto& device = GetRenderDevice();

		const auto& computeShader = cmd.Shader;

		computeShader->Bind();

		device->BindUniformBuffers(cmd.Uniforms, [](auto& buffer, const auto& binding) {
			buffer.Bind(binding.Binding);
		});

		device->BindStorageBuffers(cmd.SSBOs, [](auto& buffer, const auto& binding) {
			buffer.Bind(binding.Binding);
		});

		device->BindImages(cmd.Images, [&](const auto& image, auto binding) {
			computeShader->BindImage(image, binding.Access);
		});	

		computeShader->Dispatch(cmd.NumWorkGroup.X, cmd.NumWorkGroup.Y, cmd.NumWorkGroup.Z);

		computeShader->Unbind();
	}

	void OpenGLRenderer::BeginFrame() {
		const auto& window = GetWindow();
		const auto& device = GetRenderDevice();

		auto [width, height] = window->GetWindowSize();

		device->ClearBuffer();
		device->SetViewport(0, 0, width, height);
		device->SetClearColor(0.5f, 0.5f, 0.5f, 1.0f);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void OpenGLRenderer::SetMemoryBarrier(MemoryBarrier barrier) {
		glMemoryBarrier(static_cast<GLbitfield>(barrier));
	}

	void OpenGLRenderer::RenderToSwapChain(const std::shared_ptr<Texture>& input) {
		const auto& window = GetWindow();
		const auto& device = GetRenderDevice();
		auto [width, height] = window->GetWindowSize();

		m_FullscreenShader->Bind();
		glBindVertexArray(m_FullscrenVAO);
		m_FullscreenShader->BindImage("u_OutputTexture", input, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		m_FullscreenShader->Unbind();
	}

	void OpenGLRenderer::EndFrame() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(static_cast<GLFWwindow*>(GetWindow()->GetNativeWindow()));
	}

	void OpenGLRenderer::Resize(uint32_t newWidth, uint32_t newHeight) {
		
	}

	void OpenGLRenderer::Shutdown() {
		GetRenderContext()->Shutdown();
		ImGui::DestroyContext();
	}
}