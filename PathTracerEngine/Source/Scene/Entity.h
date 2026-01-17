#pragma once

#include "Core/Base.h"

#include "Mesh.h"

#include "glm/gtx/quaternion.hpp"
#include "glm/glm.hpp"

namespace PathTracer {

    class Entity final {
    public:
        Entity(uint64_t entityID, const std::filesystem::path& path, const std::shared_ptr<RenderDevice>& device)
            : m_EntityID(entityID), m_Mesh(entityID, path, device) {
        }
        ~Entity() = default;

        inline const std::string& GetName() const { return m_Mesh.GetName(); }

        inline const glm::vec3& GetPosition() const { return m_Position; }
        inline const glm::vec3& GetRotation() const { return m_Rotation; }
        inline const glm::vec3& GetScale() const { return m_Scale; }

        inline const Mesh& GetMesh() const { return m_Mesh; }

        inline const glm::mat4& CalculateModelMatrix() {
            glm::quat q (glm::radians(m_Rotation));
            m_Transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::mat4_cast(q) * glm::scale(glm::mat4(1.0f), m_Scale);
            return m_Transform;
        }

        inline auto GetTransform() { return std::tie(m_Position, m_Rotation, m_Scale); }
    private:
        glm::vec3 m_Position{};
        glm::vec3 m_Scale{1.0f};
        glm::vec3 m_Rotation{};

        glm::mat4 m_Transform = glm::mat4(1.0f);

        Mesh m_Mesh;
        uint64_t m_EntityID;
    };
}

