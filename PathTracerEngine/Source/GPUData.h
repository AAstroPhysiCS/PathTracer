#pragma once

#include "glm/glm.hpp"

#include "Scene/BVH.h"

namespace PathTracer {

	struct alignas(16) CameraBufferGPUData {
		glm::vec3 Position;  float _p0;
		glm::vec3 Direction; float _p1;
		glm::vec3 Right;     float _p2;
		glm::vec3 Up;

		float Fov;
	};

	struct SettingsBufferGPUData {
		uint32_t FrameCounter;
		uint32_t TotalTriangleCount;
		uint32_t MaxBounces;
		uint32_t SamplesPerFrame;

		uint32_t MaxSamples;
		uint32_t Accumulate;
		uint32_t UseMIS;
		uint32_t EnableDenoiser;

		uint32_t ShowBVH;
		uint32_t ShowRays;
		float Exposure;
		float Gamma;
	};

	struct alignas(16) BVHGPUData {
		glm::vec4 MinLeftOrCount, MaxRightOrOffset;
		int32_t IsLeaf;         // bool for inner node or leaf node

		BVHGPUData() = default;
		BVHGPUData(const BVHNode& node) 
			: MinLeftOrCount({ node.AABB.Min, node.LeftOrCount }), 
			MaxRightOrOffset({ node.AABB.Max, node.RightOrOffset }), 
			IsLeaf(node.IsLeaf) {
		}
	};

	struct alignas(16) TriangleBufferGPUData {
		glm::vec4 V0;
		glm::vec4 V1;
		glm::vec4 V2;
		
		glm::vec4 N0;
		glm::vec4 N1;
		glm::vec4 N2;
		
		glm::vec4 T0;
		glm::vec4 T1;
		glm::vec4 T2;
		
		glm::vec4 UV0{};
		glm::vec4 UV1{};
		glm::vec4 UV2{};

		glm::uvec2 ID; //x = MaterialID, y = EntityID
	};

	struct EntityGPUData {
		glm::mat4 Transform = glm::mat4(1.0f);
		glm::mat4 InverseTransposeTransform = glm::mat4(1.0f);
	};

	struct MaterialBufferGPUData {
		glm::vec4 DiffuseColor;
		glm::vec4 EmissiveColor;
		
		uint64_t AlbedoIndex;
		uint64_t NormalIndex;
		uint64_t RoughnessIndex;
		uint64_t MetallicIndex;
		uint64_t AmbientOcclusionIndex;
		uint64_t EmissionIndex;

		float Roughness;
		float Metallic;
		float padding[2];
	};

	struct TextureHandleGPUData {
		uint64_t TextureHandle;
	};
}