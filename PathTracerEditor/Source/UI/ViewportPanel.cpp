#include "ViewportPanel.h"

#include "Core/Event.h"
#include "imgui.h"

ViewportPanel::ViewportPanel(const std::shared_ptr<PathTracer::Window>& window, PathTracer::Scene& scene)
    : m_Window(window), m_Scene(scene) {
    PathTracer::EventSystem::AddEventListener<PathTracer::KeyEvent>([&](const PathTracer::KeyEvent& evt) {
        if (evt.GetKey() == GLFW_KEY_ESCAPE && evt.GetAction() == GLFW_PRESS && m_MouseCaptured) {
            m_MouseCaptured = false;
            glfwSetInputMode(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            m_Scene.GetCamera().UpdateOverride(true);
        }
    });
}

void ViewportPanel::Begin() const {
    if (!ImGui::Begin("Viewport")) {
        return;
    }

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    if (m_ViewportTexture && (viewportSize.x != m_ViewportTexture->GetWidth() || viewportSize.y != m_ViewportTexture->GetHeight())) {
        PathTracer::EventSystem::RegisterEvent<PathTracer::FramebufferResizeEvent>(nullptr, static_cast<int32_t>(viewportSize.x), static_cast<int32_t>(viewportSize.y));
    }

    if (m_ViewportTexture) {
        ImGui::Image((void*)(intptr_t)m_ViewportTexture->GetID(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            m_Scene.GetCamera().UpdateOverride(false);
            m_MouseCaptured = true;
            glfwSetInputMode(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        }
    }
}

void ViewportPanel::End() const {
    ImGui::End();
}

void ViewportPanel::SetViewportTexture(std::shared_ptr<PathTracer::Texture> viewportTexture) {
    m_ViewportTexture = viewportTexture;
}
