#include "RendererSettingsPanel.h"

#include "imgui.h"

PathTracer::RendererSettings& RendererSettingsPanel::GetSettings() {
    return s_Settings;
}

PathTracer::RendererStats& RendererSettingsPanel::GetStats() {
    return s_Stats;
}

RendererSettingsPanel::RendererSettingsPanel(uint32_t& frameCounter, PathTracer::DirectionalLight& light)
    : m_FrameCounter(frameCounter), m_DirLight(light) {
}

void RendererSettingsPanel::Begin() const {
    if (!ImGui::Begin("Renderer Settings")) {
        return;
    }

    if (ImGui::DragFloat3("World Sun Position", &m_DirLight.Direction.x, 0.1f)) m_FrameCounter = 0;
    if (ImGui::DragFloat("World Sun Intensity", &m_DirLight.Intensity, 0.1f)) m_FrameCounter = 0;

    if (ImGui::CollapsingHeader("Path Tracer", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("Accumulate", &s_Settings.Accumulate)) m_FrameCounter = 0;
        if (ImGui::DragInt("Samples / Frame", &s_Settings.SamplesPerFrame, 1, 1, 64)) m_FrameCounter = 0;
        if (ImGui::DragInt("Max Samples", &s_Settings.MaxSamples, 1, 1, 100000)) m_FrameCounter = 0;
        if (ImGui::DragInt("Max Bounces", &s_Settings.MaxBounces, 1, 1, 64)) m_FrameCounter = 0;

        ImGui::Spacing();
        if (ImGui::Checkbox("Multiple Importance Sampling", &s_Settings.UseMIS)) m_FrameCounter = 0;
        if (ImGui::Checkbox("Denoiser (Preview)", &s_Settings.EnableDenoiser)) m_FrameCounter = 0;

        ImGui::Spacing();
        if (ImGui::Button("Reset Accumulation")) m_FrameCounter = 0;
    }

    if (ImGui::CollapsingHeader("Post Processing", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::DragFloat("Exposure", &s_Settings.Exposure, 0.01f, 0.01f, 10.0f)) m_FrameCounter = 0;
        if (ImGui::DragFloat("Gamma", &s_Settings.Gamma, 0.01f, 1.0f, 3.0f)) m_FrameCounter = 0;
    }

    if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Show BVH", &s_Settings.ShowBVH);
        ImGui::Checkbox("Show Rays", &s_Settings.ShowRays);
    }

    if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Samples Accumulated: %d", s_Stats.SamplesAccumulated);
        ImGui::Text("Frame Time: %.2f ms", s_Stats.FrameTimeMS);
        ImGui::Text("GPU Memory Usage: %.2f MB", s_Stats.GPUMemoryMB);
    }
}

void RendererSettingsPanel::End() const {
    ImGui::End();
}
