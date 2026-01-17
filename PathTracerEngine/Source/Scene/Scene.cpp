#include "Scene.h"
#include "Mesh.h"

namespace PathTracer {
	
	const std::shared_ptr<Entity>& Scene::LoadMesh(const std::filesystem::path& path, const std::shared_ptr<RenderDevice>& device) {
		const auto& e = m_Entities.emplace_back(std::make_shared<Entity>(m_Entities.size(), path, device));
		m_TotalVerticesCount += e->GetMesh().GetTriangleCount();

		return e;
	}

	void Scene::Begin(const std::shared_ptr<Window>& window, double deltaTime) {
		m_Camera.Update(window, deltaTime);
	}

	void Scene::End() {
		m_Camera.End();
	}
}
