#pragma once
#include "Core/Base.h"
#include "RendererAPI.h"

#include "UIPanel.h"

struct GLFWwindow;

namespace PathTracer {

	struct WindowCreateInfo {
		std::string_view Title;
		uint32_t Width;
		uint32_t Height;

		RendererAPI TargetAPI;
	};

	class Window {
	public:
		Window(const WindowCreateInfo& createInfo);
		virtual ~Window() = default;

		virtual void Begin(double deltaTime);
		virtual void Initialize() = 0;
		virtual void RenderUI() = 0;
		virtual void Shutdown() = 0;
		virtual void End();

		inline uint32_t GetWidth() const { return m_CreateInfo.Width; }
		inline uint32_t GetHeight() const { return m_CreateInfo.Height; }

		inline auto GetFramebufferSize() const { return std::make_pair(m_FramebufferWidth, m_FramebufferHeight); }
		inline auto GetWindowSize() const { return std::make_pair(m_CreateInfo.Width, m_CreateInfo.Height); }

		inline RendererAPI GetTargetAPI() const { return m_CreateInfo.TargetAPI; }
		inline void* GetNativeWindow() const { return m_NativeWindow; }

		inline auto GetMousePosition() const { return std::make_pair(m_XPos, m_YPos); }

		void SetTitle(std::string_view title);
		inline std::string_view GetTitle() const { return m_CreateInfo.Title; }
		bool ShouldClose() const;


		template <IsPanel TPanel, typename ... TArgs>
		void AddUIPanel(TArgs&& ... args) {
			m_UIPanels.emplace_back(std::make_shared<TPanel>(std::forward<TArgs>(args)...));
		}
		
		template <IsPanel TPanel>
		std::shared_ptr<TPanel> GetUIPanel() const {
			for (auto& panel : m_UIPanels) {
				if (auto casted = std::dynamic_pointer_cast<TPanel>(panel)) {
					return casted;
				}
			}
			return nullptr;
		}

		static inline double GetDeltaTime() { return s_DeltaTime; }
	protected:
		inline const std::vector<std::shared_ptr<UIPanel>>& GetUIPanels() const { return m_UIPanels; }

		void SetNativeWindow(void* nativeWindow) { m_NativeWindow = nativeWindow; }
	private:
		WindowCreateInfo m_CreateInfo;
		void* m_NativeWindow = nullptr;
		bool m_ShouldClose = false;

		std::vector<std::shared_ptr<UIPanel>> m_UIPanels;

		uint32_t m_FramebufferWidth = 0;
		uint32_t m_FramebufferHeight = 0;

		double m_XPos = 0.0;
		double m_YPos = 0.0;

		inline static double s_DeltaTime = 0.0;
	};

	class Win32Window final : public Window {
	public:
		Win32Window(const WindowCreateInfo& createInfo);
		virtual ~Win32Window() = default;

		void Initialize();
		void Shutdown();
		void RenderUI();
	};

	static void ErrorCallback(int error, const char* description);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void WindowSizeCallback(GLFWwindow* window, int width, int height);
}