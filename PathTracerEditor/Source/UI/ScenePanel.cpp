#include "ScenePanel.h"

#include "Scene/Scene.h"

#include "imgui.h"

#include "ImGuiFileDialog.h"

#include "RenderDevice.h"

ScenePanel::ScenePanel(uint32_t& frameCounter, PathTracer::Scene& scene, const std::shared_ptr<PathTracer::RenderDevice>& device)
    : m_FrameCounter(frameCounter), m_Scene(scene), m_RenderDevice(device) {
}

void ScenePanel::Begin() const {
    ImGui::Begin("Scene");

    if (ImGui::BeginPopupContextWindow("SceneContextMenu",
        ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) {

        if (ImGui::MenuItem("Create Empty Entity")) {
            //m_SelectedEntity = m_Scene.CreateEntity("Empty Entity");
        }

        if (ImGui::MenuItem("Create Entity from Mesh")) {
            ImGuiFileDialog::Instance()->OpenDialog("LoadMeshDialog", "Load Mesh", ".gltf,.fbx,.glb,.obj");
        }

        ImGui::EndPopup();
    }

    DrawEntityList();
    ImGui::Separator();
    DrawEntityInspector();

    HandleMeshLoading();
}

void ScenePanel::End() const {
    ImGui::End();
}

void ScenePanel::DrawEntityList() const {
    ImGui::Text("Entities");

    const auto& allEntites = m_Scene.GetAllEntities();
    for (int32_t i = 0; const auto& entity : allEntites) {
        bool selected = (m_SelectedEntity == entity);

        ImGui::PushID(i++);
        if (ImGui::Selectable(entity->GetName().c_str(), selected)) {
            m_SelectedEntity = entity;

            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Delete")) {
                    //m_Scene.DestroyEntity(entity);
                    if (m_SelectedEntity == entity)
                        m_SelectedEntity = nullptr;
                }
                ImGui::EndPopup();
            }
        }
        ImGui::PopID();
    }
}

void ScenePanel::DrawEntityInspector() const {
    if (!m_SelectedEntity) {
        ImGui::TextDisabled("No entity selected");
        return;
    }

    ImGui::Text("View");
    ImGui::Separator();

    auto&& [pos, rot, scale] = m_SelectedEntity->GetTransform();

    if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
        m_FrameCounter = 0;
    if (ImGui::DragFloat3("Rotation", &rot.x, 0.1f))
        m_FrameCounter = 0;
    if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
        m_FrameCounter = 0;
}

void ScenePanel::HandleMeshLoading() const {
    if (ImGuiFileDialog::Instance()->Display("LoadMeshDialog")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            const auto& entity = m_Scene.LoadMesh(filePath, m_RenderDevice);
            m_SelectedEntity = entity;
        }

        ImGuiFileDialog::Instance()->Close();
    }
}
