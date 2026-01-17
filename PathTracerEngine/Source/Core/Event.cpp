#include "Event.h"

namespace PathTracer {

	KeyEvent::KeyEvent(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods, double deltaTime) 
		: m_Window(window), m_Key(key), m_Scancode(scancode), m_Action(action), m_Mods(mods), m_DeltaTime(deltaTime) {
	}

	MouseCursorEvent::MouseCursorEvent(GLFWwindow* window, double xPos, double yPos) 
		: m_Window(window), m_XPos(xPos), m_YPos(yPos) {
	}

	MouseButtonEvent::MouseButtonEvent(GLFWwindow* window, int32_t button, int32_t action, int32_t mods) 
		: m_Window(window), m_Button(button), m_Action(action), m_Mods(mods) {
	}

	FramebufferResizeEvent::FramebufferResizeEvent(GLFWwindow* window, int32_t width, int32_t height)
		: m_Window(window), m_Width(width), m_Height(height) {
	}

	WindowResizeEvent::WindowResizeEvent(GLFWwindow* window, int32_t width, int32_t height)
		: m_Window(window), m_Width(width), m_Height(height) {
	}

	void EventSystem::ProcessEvents() {
		s_KeyEventRegister.Process();
		s_MouseCursorEventRegister.Process();
		s_MouseButtonEventRegister.Process();
		s_FramebufferResizeRegister.Process();
		s_WindowResizeRegister.Process();
	}
}
