#include "Mesh.h"

#include "RenderDevice.h"

namespace PathTracer {

	static constexpr const uint32_t FLAGS = 
		aiProcess_Triangulate			| 
		aiProcess_GenSmoothNormals		| 
		aiProcess_FlipUVs				| 
		aiProcess_CalcTangentSpace		| 
		aiProcess_GenBoundingBoxes		| 
		aiProcess_JoinIdenticalVertices | 
		aiProcess_FixInfacingNormals	|
		aiProcess_GenUVCoords			| 
		aiProcess_OptimizeMeshes;

	Mesh::Mesh(uint64_t entityID, const std::filesystem::path& path, const std::shared_ptr<RenderDevice>& device)
		: m_EntityID(entityID), m_Path(path), m_RenderDevice(device) {
#ifdef DEBUG
		const aiScene* scene = m_Importer.ReadFile(path.string(), aiProcessPreset_TargetRealtime_Fast | FLAGS);
#else
		const aiScene* scene = m_Importer.ReadFile(path.string(), aiProcessPreset_TargetRealtime_MaxQuality | FLAGS);
#endif
		PT_ASSERT(scene, std::format("Mesh loading failed at path {}", path.string()));

		if (!scene)
			return;

		m_Materials.resize(scene->mNumMaterials);
		m_MaterialBufferGPUs.resize(scene->mNumMaterials);

		ImportScene(scene, scene->mRootNode, scene->mRootNode->mTransformation);

		for (const auto& submesh : m_Submeshes) {
			for (size_t j = 0; j < submesh.IndexCount; j += 3) {
				TriangleBufferGPUData tri;
				size_t i0 = submesh.Faces[j + 0];
				size_t i1 = submesh.Faces[j + 1];
				size_t i2 = submesh.Faces[j + 2];

				tri.V0 = glm::vec4(submesh.Vertices[i0], 1.0f);
				tri.V1 = glm::vec4(submesh.Vertices[i1], 1.0f);
				tri.V2 = glm::vec4(submesh.Vertices[i2], 1.0f);

				glm::vec3 c = (tri.V0 + tri.V1 + tri.V2) / 3.0f;
				tri.V0.w = c.x;
				tri.V1.w = c.y;
				tri.V2.w = c.z;

				tri.N0 = glm::vec4(submesh.Normals[i0], 1.0f);
				tri.N1 = glm::vec4(submesh.Normals[i1], 1.0f);
				tri.N2 = glm::vec4(submesh.Normals[i2], 1.0f);

				glm::vec3 min = glm::min(glm::min(glm::vec3(tri.V0), glm::vec3(tri.V1)), glm::vec3(tri.V2));
				glm::vec3 max = glm::max(glm::max(glm::vec3(tri.V0), glm::vec3(tri.V1)), glm::vec3(tri.V2));

				tri.N0.w = min.x;
				tri.N1.w = min.y;
				tri.N2.w = min.z;

				if (!submesh.Tangents.empty()) {
					tri.T0 = glm::vec4(submesh.Tangents[i0], 1.0f);
					tri.T1 = glm::vec4(submesh.Tangents[i1], 1.0f);
					tri.T2 = glm::vec4(submesh.Tangents[i2], 1.0f);

					tri.T0.w = max.x;
					tri.T1.w = max.y;
					tri.T2.w = max.z;
				}

				if (!submesh.TextureCoords.empty()) {
					tri.UV0 = glm::vec4(submesh.TextureCoords[i0], 1.0f, 1.0f);
					tri.UV1 = glm::vec4(submesh.TextureCoords[i1], 1.0f, 1.0f);
					tri.UV2 = glm::vec4(submesh.TextureCoords[i2], 1.0f, 1.0f);
				}

				tri.ID = glm::uvec2(submesh.MaterialID, m_EntityID);

				m_TriangleBufferGPUs.push_back(tri);
			}
		}

		m_BVHGPUs.reserve(m_TriangleCount * 2);
		SubdivideBVH(0, (uint32_t)m_TriangleCount);

		m_Importer.FreeScene();
	}

	AABB Mesh::ComputeAABB(uint32_t triHead, uint32_t triCount) {
		AABB box{};

		for (uint32_t i = 0; i < triCount; i++) {
			const TriangleBufferGPUData& tri = m_TriangleBufferGPUs[triHead + i];

			const glm::vec3& v0 = tri.V0;
			const glm::vec3& v1 = tri.V1;
			const glm::vec3& v2 = tri.V2;

			box.Min = glm::min(box.Min, glm::vec3(tri.N0.w, tri.N1.w, tri.N2.w));
			box.Max = glm::max(box.Max, glm::vec3(tri.T0.w, tri.T1.w, tri.T2.w));
		}

		return box;
	}

	/*
		From: https://jacco.ompf2.com/2022/04/21/how-to-build-a-bvh-part-3-quick-builds/
	*/
	float Mesh::FindBestSplitPlane(uint32_t triHead, uint32_t triCount, uint32_t& axis, float& splitPos) {
		float bestCost = 1e30f;
		for (int a = 0; a < 3; a++) {
			float boundsMin = 1e30f, boundsMax = -1e30f;

			for (uint32_t i = 0; i < triCount; i++) {
				const TriangleBufferGPUData& tri = m_TriangleBufferGPUs[triHead + i];
				boundsMin = glm::min(boundsMin, glm::vec3(tri.V0.w, tri.V1.w, tri.V2.w)[a]);
				boundsMax = glm::max(boundsMax, glm::vec3(tri.V0.w, tri.V1.w, tri.V2.w)[a]);
			}

			if (boundsMin == boundsMax) continue;

			Bin bin[BINS];
			float scale = BINS / (boundsMax - boundsMin);
			for (uint32_t i = 0; i < triCount; i++) {
				const TriangleBufferGPUData& tri = m_TriangleBufferGPUs[triHead + i];
				glm::vec3 centroid = glm::vec3(tri.V0.w, tri.V1.w, tri.V2.w);
				int32_t binIdx = glm::min(BINS - 1, static_cast<uint32_t>((centroid[a] - boundsMin) * scale));

				bin[binIdx].TriangleCount++;
				bin[binIdx].Bounds.Min = glm::min(bin[binIdx].Bounds.Min,
					glm::vec3(tri.N0.w, tri.N1.w, tri.N2.w));
				bin[binIdx].Bounds.Max = glm::max(bin[binIdx].Bounds.Max,
					glm::vec3(tri.T0.w, tri.T1.w, tri.T2.w));
			}

			// gather data for the 7 planes between the 8 bins
			float leftArea[BINS - 1], rightArea[BINS - 1];
			int32_t leftCount[BINS - 1], rightCount[BINS - 1];

			AABB leftBox, rightBox;
			int leftSum = 0, rightSum = 0;
			for (int i = 0; i < BINS - 1; i++) {
				leftSum += bin[i].TriangleCount;
				leftCount[i] = leftSum;
				leftBox.Grow(bin[i].Bounds);
				leftArea[i] = leftBox.Area();

				rightSum += bin[BINS - 1 - i].TriangleCount;
				rightCount[BINS - 2 - i] = rightSum;
				rightBox.Grow(bin[BINS - 1 - i].Bounds);
				rightArea[BINS - 2 - i] = rightBox.Area();
			}

			// calculate SAH cost for the 7 planes
			scale = (boundsMax - boundsMin) / BINS;
			for (int i = 0; i < BINS - 1; i++) {
				float planeCost = leftCount[i] * leftArea[i] + rightCount[i] * rightArea[i];
				if (planeCost < bestCost) {
					axis = a; 
					splitPos = boundsMin + scale * (i + 1);
					bestCost = planeCost;
				}
			}
		}
		return bestCost;
	}

	/*
		From: https://jacco.ompf2.com/2022/04/18/how-to-build-a-bvh-part-2-faster-rays/
	*/
	uint32_t Mesh::SubdivideBVH(uint32_t firstTri, uint32_t triCount) {
		uint32_t nodeIndex = (uint32_t)m_BVHGPUs.size();
		BVHGPUData& data = m_BVHGPUs.emplace_back();
		BVHNode node{};

		node.AABB = ComputeAABB(firstTri, triCount);

		if (triCount <= 2) {
			node.IsLeaf = true;
			node.LeftOrCount = triCount;
			node.RightOrOffset = firstTri;

			data = BVHGPUData(node);

			return nodeIndex;
		}

		AABB aabb = node.AABB;
		glm::vec3 extent = aabb.Max - aabb.Min;

		float parentArea = 2.0f * (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x);
		float parentCost = triCount * parentArea;

		uint32_t bestAxis = 0;
		float bestPos = 0.0f;
		float bestCost = FindBestSplitPlane(firstTri, triCount, bestAxis, bestPos);

		// Abort split if no improvement
		if (bestCost >= parentCost) {
			node.IsLeaf = true;
			node.LeftOrCount = triCount;
			node.RightOrOffset = firstTri;
			data = BVHGPUData(node);
			return nodeIndex;
		}

		uint32_t i = firstTri;
		uint32_t j = firstTri + triCount - 1;

		while (i <= j) {
			const TriangleBufferGPUData& tri = m_TriangleBufferGPUs[i];
			glm::vec3 centroid(tri.V0.w, tri.V1.w, tri.V2.w);

			if (centroid[bestAxis] < bestPos)
				i++;
			else {
				std::swap(m_TriangleBufferGPUs[i], m_TriangleBufferGPUs[j]);
				j--;
			}
		}

		uint32_t leftCount = i - firstTri;
		if (leftCount == 0 || leftCount == triCount)
			leftCount = triCount / 2;

		// Build children AFTER reserving parent slot
		node.LeftOrCount = SubdivideBVH(firstTri, leftCount);
		node.RightOrOffset = SubdivideBVH(firstTri + leftCount, triCount - leftCount);
		node.IsLeaf = false;

		data = BVHGPUData(node);

		return nodeIndex;
	}

	void Mesh::ImportScene(const aiScene* scene, const aiNode* node, const aiMatrix4x4& parentTransformation) {
		aiMatrix4x4 m = parentTransformation * node->mTransformation;

		auto transform = glm::mat4(
			m.a1, m.a2, m.a3, m.a4,
			m.b1, m.b2, m.b3, m.b4,
			m.c1, m.c2, m.c3, m.c4,
			m.d1, m.d2, m.d3, m.d4);

		//just to be safe, copy it
		m_Name = std::string(node->mName.C_Str());

		uint32_t baseVertexCount = 0;
		uint32_t baseIndexCount = 0;

		for (size_t i = 0; i < node->mNumMeshes; i++) {
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			Submesh submesh;
			submesh.BaseVertexCount = baseVertexCount;
			submesh.BaseIndexCount = baseIndexCount;
			submesh.VertexCount = mesh->mNumVertices;
			submesh.IndexCount = mesh->mNumFaces * 3;

			baseVertexCount += submesh.VertexCount;
			baseIndexCount += submesh.IndexCount;

			AABB& aabb = m_SubmeshAABBs.emplace_back(mesh->mAABB);
			aabb.Min = transform * glm::vec4(aabb.Min, 1.0f);
			aabb.Max = transform * glm::vec4(aabb.Max, 1.0f);

			uint32_t sizeVertices = submesh.VertexCount;
			m_TriangleCount += submesh.IndexCount / 3;

			if (mesh->HasPositions()) {
				aiVector3D* vertices = mesh->mVertices;
				submesh.Vertices.resize(sizeVertices);
				memcpy(submesh.Vertices.data(), vertices, sizeVertices * sizeof(aiVector3D));

				for (uint32_t v = 0; v < submesh.Vertices.size(); v++) {
					glm::vec4 p(submesh.Vertices[v].x, submesh.Vertices[v].y, submesh.Vertices[v].z, 1.0f);
					p = transform * p;
					submesh.Vertices[v] = { p.x, p.y, p.z };
				}
			}

			if (mesh->HasNormals()) {
				aiVector3D* normals = mesh->mNormals;
				submesh.Normals.resize(sizeVertices);

				memcpy(submesh.Normals.data(), normals, sizeVertices * sizeof(aiVector3D));
				glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));

				for (uint32_t v = 0; v < submesh.Normals.size(); v++) {
					glm::vec3 n(submesh.Normals[v].x, submesh.Normals[v].y, submesh.Normals[v].z);

					n = glm::normalize(normalMatrix * n);
					submesh.Normals[v] = { n.x, n.y, n.z };
				}
			}

			if (mesh->HasTextureCoords(0)) {
				aiVector3D* textureCoords = mesh->mTextureCoords[0];
				submesh.TextureCoords.reserve(sizeVertices);
				for (uint32_t j = 0; j < sizeVertices; j++) {
					submesh.TextureCoords.emplace_back(textureCoords[j].x, textureCoords[j].y);
				}
			}

			if (mesh->HasTangentsAndBitangents()) {
				aiVector3D* tangents = mesh->mTangents;
				submesh.Tangents.resize(sizeVertices);
				memcpy(submesh.Tangents.data(), tangents, sizeVertices * sizeof(aiVector3D));

				aiVector3D* biTangents = mesh->mBitangents;
				submesh.BiTangents.resize(sizeVertices);
				memcpy(submesh.BiTangents.data(), biTangents, sizeVertices * sizeof(aiVector3D));
			}

			if (mesh->HasFaces()) {
				submesh.Faces.reserve(submesh.IndexCount);

				for (uint32_t j = 0; j < mesh->mNumFaces; j++) {
					aiFace aiFace = mesh->mFaces[j];
					for (uint32_t k = 0; k < aiFace.mNumIndices; k++) {
						submesh.Faces.emplace_back(aiFace.mIndices[k]);
					}
				}
			}

			submesh.MaterialID = mesh->mMaterialIndex;

			m_Materials[submesh.MaterialID] = ImportMaterial(scene->mMaterials[submesh.MaterialID], submesh.MaterialID);
			m_Submeshes.push_back(submesh);
		}

		for (size_t i = 0; i < node->mNumChildren; i++)
			ImportScene(scene, node->mChildren[i], m);
	}

	MeshMaterial Mesh::ImportMaterial(const aiMaterial* mtl, uint32_t id) {
		MeshMaterial output;

		aiColor4D diffuseColor;
		aiColor3D emissive(1.0f, 1.0f, 1.0f);
		uint64_t albedoIndex = 0, normalIndex = 0, roughnessIndex = 0, metallicIndex = 0, ambientOcclusionIndex = 0, emissiveIndex = 0;

		int32_t texIndex = 0;
		aiString texPath;

		const auto root = m_Path.parent_path();

		if (AI_SUCCESS == mtl->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) ||
			AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)) {
			//TODO: do this better -> very wasteful
			output.Diffuse = m_RenderDevice->CreateTexture({ .Path = root / texPath.C_Str(), .Format = TextureFormat::SRGBA8 });
			albedoIndex = m_MaterialBufferGPUs.empty() ? 1 : output.Diffuse->GetHandle();
		} else {
			if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor)) {
				output.DiffuseColor = glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
			}
		}	

		if (AI_SUCCESS == mtl->GetTexture(aiTextureType_NORMALS, 0, &texPath)) {
			output.Normal = m_RenderDevice->CreateTexture({ .Path = root / texPath.C_Str(), .Format = TextureFormat::RGB8_UNorm });
			normalIndex = m_MaterialBufferGPUs.empty() ? 2 : output.Normal->GetHandle();
		}

		if (AI_SUCCESS == mtl->GetTexture(aiTextureType_GLTF_METALLIC_ROUGHNESS, 0, &texPath)) {
			output.MetallicRoughness = m_RenderDevice->CreateTexture({ .Path = root / texPath.C_Str(), .Format = TextureFormat::RG8_UNorm });
			metallicIndex = m_MaterialBufferGPUs.empty() ? 3 : output.MetallicRoughness->GetHandle();
		} else {
			mtl->Get(AI_MATKEY_ROUGHNESS_FACTOR, output.RoughnessFactor);
			mtl->Get(AI_MATKEY_METALLIC_FACTOR, output.MetallicFactor);

			if (output.RoughnessFactor == 0.0f)
				mtl->Get(AI_MATKEY_SHININESS, output.RoughnessFactor);
			if (output.MetallicFactor == 0.0f)
				mtl->Get(AI_MATKEY_SPECULAR_FACTOR, output.MetallicFactor);
		}

		if (AI_SUCCESS == mtl->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texPath)) {
			output.AmbientOcclusion = m_RenderDevice->CreateTexture({ .Path = root / texPath.C_Str(), .Format = TextureFormat::R8_UNorm });
			ambientOcclusionIndex = m_MaterialBufferGPUs.empty() ? 4 : output.AmbientOcclusion->GetHandle();
		}

		if (AI_SUCCESS == mtl->GetTexture(aiTextureType_EMISSIVE, 0, &texPath)) {
			output.Emissive = m_RenderDevice->CreateTexture({ .Path = root / texPath.C_Str(), .Format = TextureFormat::RGBA8_UNorm });
			emissiveIndex = m_MaterialBufferGPUs.empty() ? 5 : output.Emissive->GetHandle();
		} else {
			if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_EMISSIVE, emissive)) {
				output.EmissiveColor = glm::vec4(emissive.r, emissive.g, emissive.b, 0.0f);
			}
		}

		m_MaterialBufferGPUs[id] = MaterialBufferGPUData{
			.DiffuseColor = output.DiffuseColor,
			.EmissiveColor = output.EmissiveColor,
			.AlbedoIndex = albedoIndex,
			.NormalIndex = normalIndex,
			.RoughnessIndex = roughnessIndex,
			.MetallicIndex = metallicIndex,
			.AmbientOcclusionIndex = ambientOcclusionIndex,
			.EmissionIndex = emissiveIndex,
			.Roughness = output.RoughnessFactor == 0.0f ? 1.0f : output.RoughnessFactor,
			.Metallic = output.MetallicFactor,
		};

		return output;
	}
}
