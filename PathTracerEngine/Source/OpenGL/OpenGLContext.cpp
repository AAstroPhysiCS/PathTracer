#include "OpenGLContext.h"

#include "Core/Window.h"

#define GLAD_GL_IMPLEMENTATION
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace PathTracer {

	OpenGLContext::OpenGLContext(const RenderContextCreateInfo& createInfo)
		: RenderContext(createInfo) {
	}

	void OpenGLContext::Initialize() {
		const auto& window = GetWindow();

		glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->GetNativeWindow()));
		glfwSwapInterval(IsVsyncEnabled());

		PT_ASSERT(gladLoadGL(), "OpenGLContext: Failed to initialize GLAD!");

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(OpenGLDebugCallback, nullptr);

		glDebugMessageControl(
			GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			nullptr,
			GL_TRUE
		);

		int32_t flags = 0;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

		PT_ASSERT(flags & GL_CONTEXT_FLAG_DEBUG_BIT, "OpenGL debug context NOT enabled");

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window->GetNativeWindow()), true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void OpenGLContext::Shutdown() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	void GLAPIENTRY OpenGLContext::OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			return;

		PT_LOG_CRITICAL("OpenGL Debug Message:");
		PT_LOG_CRITICAL(std::format("  Message: {}", message));
		PT_LOG_CRITICAL(std::format("  Type: {}", type));
		PT_LOG_CRITICAL(std::format("  Severity: {}", severity));

		PT_ASSERT(false, "OpenGL error");
	}
}