#pragma once

#include "Entity.h"
#include "Core/Window.h"

#include "Camera.h"

namespace PathTracer {

	class Scene final {
	public:
		Scene() = default;
		~Scene() = default;

		const std::shared_ptr<Entity>& LoadMesh(const std::filesystem::path& path, const std::shared_ptr<RenderDevice>& device);

		void Begin(const std::shared_ptr<Window>& window, double deltaTime);
		void End();

		inline FPSCamera& GetCamera() { return m_Camera; }
		inline const std::vector<std::shared_ptr<Entity>>& GetAllEntities() const { return m_Entities; }

		inline uint64_t GetTotalTriangleCount() const { return m_TotalVerticesCount; }
	private:
		FPSCamera m_Camera{ glm::vec3{ 0.0f, 0.0f, 0.0f }, 90.0f, 0.1f, 1000.0f };
		std::vector<std::shared_ptr<Entity>> m_Entities;

		uint64_t m_TotalVerticesCount = 0;
	};
}