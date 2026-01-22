#pragma once
#include "Core/Base.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "GPUData.h"

#include "glm/glm.hpp"

#include "BVH.h"

namespace PathTracer {

	class Texture;
	class RenderDevice;

	struct Submesh final {
		std::vector<glm::vec3> Vertices;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec3> Tangents;
		std::vector<glm::vec3> BiTangents;
		std::vector<glm::vec2> TextureCoords;

		std::vector<uint32_t> Faces;
		uint32_t MaterialID;

		glm::mat4 Transform = glm::mat4(1.0f);

		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;
		uint32_t BaseVertexCount = 0;
		uint32_t BaseIndexCount = 0;
	};

	struct MeshMaterial final {
		glm::vec4 DiffuseColor;
		float RoughnessFactor = 1.0f;
		float MetallicFactor = 0.0f;
		glm::vec4 EmissiveColor{ 0.0f };
		std::shared_ptr<Texture> Diffuse;
		std::shared_ptr<Texture> Normal;
		std::shared_ptr<Texture> MetallicRoughness;
		std::shared_ptr<Texture> AmbientOcclusion;
		std::shared_ptr<Texture> Emissive;
	};

	class Mesh final {
	private:
		struct Bin final {
			AABB Bounds{};
			uint32_t TriangleCount = 0;
		};

		inline static constexpr uint32_t BINS = 8;
	public:
		Mesh(uint64_t entityID, const std::filesystem::path& path, const std::shared_ptr<RenderDevice>& device);
		~Mesh() = default;

		inline const std::filesystem::path& GetPath() const { return m_Path; }
		inline const std::string& GetName() const { return m_Name; }

		inline const std::vector<AABB>& GetSubmeshAABBs() const { return m_SubmeshAABBs; }
		inline const std::vector<MeshMaterial>& GetMaterials() const { return m_Materials; }
		inline const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

		inline uint64_t GetTriangleCount() const { return m_TriangleCount; }
		inline const std::vector<TriangleBufferGPUData>& GetTriangleBufferGPUDatas() const { return m_TriangleBufferGPUs; }
		inline const std::vector<MaterialBufferGPUData>& GetMaterialBufferGPUDatas() const { return m_MaterialBufferGPUs; }
		inline const std::vector<BVHGPUData>& GetBVHGPUDatas() const { return m_BVHGPUs; }

		inline uint64_t GetEntityID() const { return m_EntityID; }
	private:
		AABB ComputeAABB(uint32_t triHead, uint32_t triCount);
		float FindBestSplitPlane(uint32_t triHead, uint32_t triCount, uint32_t& axis, float& splitPos);
		uint32_t SubdivideBVH(uint32_t firstTri, uint32_t triCount);
		void ImportScene(const aiScene* scene, const aiNode* node, const aiMatrix4x4& parentTransformation);
		MeshMaterial ImportMaterial(const aiMaterial* mtl, uint32_t id);

		uint64_t m_TriangleCount = 0;
		uint64_t m_EntityID = 0;

		std::string m_Name = "Unknown mesh";
		std::filesystem::path m_Path;

		std::vector<AABB> m_SubmeshAABBs;
		std::vector<MeshMaterial> m_Materials;
		std::vector<Submesh> m_Submeshes;

		std::vector<TriangleBufferGPUData> m_TriangleBufferGPUs;
		std::vector<MaterialBufferGPUData> m_MaterialBufferGPUs;

		std::vector<BVHGPUData> m_BVHGPUs;

		Assimp::Importer m_Importer;

		std::shared_ptr<RenderDevice> m_RenderDevice = nullptr;
	};
}