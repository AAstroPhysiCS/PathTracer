#pragma once

#include "Core/Base.h"

#include "GLFW/glfw3.h"

namespace PathTracer {

	enum class EventType : uint8_t {
		KeyEvent,
		MouseButtonEvent,
		MouseCursorEvent,
		FramebufferResizeEvent,
		WindowResizeEvent
	};

	class KeyEvent final {
	public:
		inline EventType GetEventType() { return EventType::KeyEvent; }
		
		KeyEvent(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods, double deltaTime);
		~KeyEvent() = default;

		inline GLFWwindow* GetWindow() const { return m_Window; }

		inline int32_t GetKey() const { return m_Key; }
		inline int32_t GetScancode() const { return m_Scancode; }
		inline int32_t GetAction() const { return m_Action; }
		inline int32_t GetMods() const { return m_Mods; }

		inline double GetDeltaTime() const { return m_DeltaTime; }
	private:
		GLFWwindow* m_Window;

		int32_t m_Key;
		int32_t m_Scancode;
		int32_t m_Action;
		int32_t m_Mods;

		double m_DeltaTime;
	};

	class MouseButtonEvent final {
	public:
		inline EventType GetEventType() { return EventType::MouseButtonEvent; }

		MouseButtonEvent(GLFWwindow* window, int32_t button, int32_t action, int32_t mods);
		~MouseButtonEvent() = default;

		inline GLFWwindow* GetWindow() const { return m_Window; }

		inline int32_t GetButton() const { return m_Button; }
		inline int32_t GetAction() const { return m_Action; }
		inline int32_t GetMods() const { return m_Mods; }
	private:
		GLFWwindow* m_Window;

		int32_t m_Button;
		int32_t m_Action;
		int32_t m_Mods;
	};

	class MouseCursorEvent final {
	public:
		inline EventType GetEventType() { return EventType::MouseCursorEvent; }

		MouseCursorEvent(GLFWwindow* window, double xPos, double yPos);
		~MouseCursorEvent() = default;

		inline GLFWwindow* GetWindow() const { return m_Window; }

		inline auto GetPos() const { return std::make_pair(m_XPos, m_YPos); }

		inline double GetXPos() const { return m_XPos; }
		inline double GetYPos() const { return m_YPos; }
	private:
		GLFWwindow* m_Window;

		double m_XPos;
		double m_YPos;
	};

	class FramebufferResizeEvent final {
	public:
		inline EventType GetEventType() { return EventType::FramebufferResizeEvent; }
		
		FramebufferResizeEvent(GLFWwindow* window, int32_t width, int32_t height);
		~FramebufferResizeEvent() = default;

		inline GLFWwindow* GetWindow() const { return m_Window; }

		inline int32_t GetWidth() const { return m_Width; }
		inline int32_t GetHeight() const { return m_Height; }
	private:
		GLFWwindow* m_Window;

		int32_t m_Width;
		int32_t m_Height;
	};

	class WindowResizeEvent final {
	public:
		inline EventType GetEventType() { return EventType::WindowResizeEvent; }

		WindowResizeEvent(GLFWwindow* window, int32_t width, int32_t height);
		~WindowResizeEvent() = default;

		inline GLFWwindow* GetWindow() const { return m_Window; }

		inline int32_t GetWidth() const { return m_Width; }
		inline int32_t GetHeight() const { return m_Height; }
	private:
		GLFWwindow* m_Window;

		int32_t m_Width;
		int32_t m_Height;
	};

	template <typename TEvent>
	using EventFunc = std::function<void(const TEvent&)>;

	template <typename TEvent>
	concept IsEvent = requires(TEvent&& event) {
		{ event.GetEventType() } -> std::convertible_to<EventType>;
	};

	template <IsEvent TEvent>
	struct EventRegister {
		std::vector<TEvent> Events;
		std::vector<EventFunc<TEvent>> Listeners;

		void Process();
	};

	class EventSystem final {
	public:
		EventSystem() = default;
		~EventSystem() = default;

		static void ProcessEvents();

		template <IsEvent TEvent>
		static void AddEventListener(EventFunc<TEvent>&& func) {
			GetRegister<TEvent>().Listeners.emplace_back(std::move(func));
		}

		template <IsEvent TEvent, typename... Args>
		static void RegisterEvent(Args&&... args) {
			GetRegister<TEvent>().Events.emplace_back(std::forward<Args>(args)...);
		}
	private:
		template <typename TEvent>
		static EventRegister<TEvent>& GetRegister();

		inline static EventRegister<KeyEvent> s_KeyEventRegister;
		inline static EventRegister<MouseCursorEvent> s_MouseCursorEventRegister;
		inline static EventRegister<MouseButtonEvent> s_MouseButtonEventRegister;
		inline static EventRegister<FramebufferResizeEvent> s_FramebufferResizeRegister;
		inline static EventRegister<WindowResizeEvent> s_WindowResizeRegister;
	};

	template<IsEvent TEvent>
	inline void EventRegister<TEvent>::Process() {
		for (const auto& event : Events) {
			for (const auto& listener : Listeners) {
				listener(event);
			}
		}

		Events.clear();
	}

	template <>
	inline EventRegister<KeyEvent>& EventSystem::GetRegister<KeyEvent>() {
		return s_KeyEventRegister;
	}

	template <>
	inline EventRegister<MouseCursorEvent>& EventSystem::GetRegister<MouseCursorEvent>() {
		return s_MouseCursorEventRegister;
	}

	template <>
	inline EventRegister<MouseButtonEvent>& EventSystem::GetRegister<MouseButtonEvent>() {
		return s_MouseButtonEventRegister;
	}

	template <>
	inline EventRegister<FramebufferResizeEvent>& EventSystem::GetRegister<FramebufferResizeEvent>() {
		return s_FramebufferResizeRegister;
	}

	template <>
	inline EventRegister<WindowResizeEvent>& EventSystem::GetRegister<WindowResizeEvent>() {
		return s_WindowResizeRegister;
	}
}