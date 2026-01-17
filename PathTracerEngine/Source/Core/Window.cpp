#include "Window.h"

#include <format>

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "Event.h"

namespace PathTracer {
	
	Window::Window(const WindowCreateInfo& createInfo)
		: m_CreateInfo(createInfo) {

		EventSystem::AddEventListener<MouseCursorEvent>([&](const MouseCursorEvent& evt) {
			m_XPos = evt.GetXPos();
			m_YPos = evt.GetYPos();
		});

		EventSystem::AddEventListener<WindowResizeEvent>([&](const WindowResizeEvent& evt) {
			//skip the init
			if (evt.GetWidth() == 0 || evt.GetHeight() == 0)
				return;
			m_CreateInfo.Width = evt.GetWidth();
			m_CreateInfo.Height = evt.GetHeight();
		});

		EventSystem::AddEventListener<FramebufferResizeEvent>([&](const FramebufferResizeEvent& evt) {
			//skip the init
			if (evt.GetWidth() == 0 || evt.GetHeight() == 0)
				return;
			m_FramebufferWidth = evt.GetWidth();
			m_FramebufferHeight = evt.GetHeight();
		});
	}

	void Window::Begin(double deltaTime) {
		s_DeltaTime = deltaTime;
	}

	void Window::End() {
		GLFWwindow* window = static_cast<GLFWwindow*>(GetNativeWindow());

		EventSystem::ProcessEvents();

		glfwPollEvents();
	}

	void Window::SetTitle(std::string_view title) {
		glfwSetWindowTitle(static_cast<GLFWwindow*>(m_NativeWindow), title.data());
	}

	Win32Window::Win32Window(const WindowCreateInfo& createInfo) 
		: Window(createInfo) {
	}

	void Win32Window::Initialize() {
		const auto& rendererAPI = GetTargetAPI();

		glfwInit();

		switch (rendererAPI) {
			case RendererAPI::OpenGL:
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
				break;
			default:
				PT_ASSERT(false, "Win32Window: Initialize failed, no supported renderer api found!");
		}

		GLFWwindow* window = glfwCreateWindow(static_cast<int32_t>(GetWidth()), static_cast<int32_t>(GetHeight()), 
			GetTitle().data(), nullptr, nullptr);
		SetNativeWindow(window);

		PT_ASSERT_AND_IF_FAILS(window, "Win32Window: GLFW window creation failed!", [this]() {
			Shutdown();
		});

		glfwSetErrorCallback(ErrorCallback);

		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseCallback);
		glfwSetCursorPosCallback(window, MouseCursorPosCallback);
		//glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
		glfwSetWindowSizeCallback(window, WindowSizeCallback);
	}

	void Win32Window::RenderUI() {
		for (auto& panel : GetUIPanels()) {
			panel->Begin();
		}
		
		for (auto& panel : GetUIPanels()) {
			panel->End();
		}
	}

	void Win32Window::Shutdown() {
		GLFWwindow* window = static_cast<GLFWwindow*>(GetNativeWindow());
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void ErrorCallback(int error, const char* description) {
		PT_LOG_CRITICAL(std::format("GLFW Error ({}): {}", error, description));
	}

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		//if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		//	glfwSetWindowShouldClose(window, GLFW_TRUE);
		EventSystem::RegisterEvent<KeyEvent>(window, key, scancode, action, mods, Window::GetDeltaTime());
	}

	void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
		EventSystem::RegisterEvent<MouseButtonEvent>(window, button, action, mods);
	}

	void MouseCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
		EventSystem::RegisterEvent<MouseCursorEvent>(window, xpos, ypos);
	}

	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
		EventSystem::RegisterEvent<FramebufferResizeEvent>(window, width, height);
	}

	void WindowSizeCallback(GLFWwindow* window, int width, int height) {
		EventSystem::RegisterEvent<WindowResizeEvent>(window, width, height);
	}

	bool Window::ShouldClose() const {
		return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_NativeWindow));
	}
}