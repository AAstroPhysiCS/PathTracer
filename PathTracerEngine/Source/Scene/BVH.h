#pragma once

#include "glm/glm.hpp"
#include "assimp/vector3.h"
#include "assimp/aabb.h"

namespace PathTracer {

	struct AABB final {
		glm::vec3 Min = glm::vec3{ std::numeric_limits<float>::max() };
		glm::vec3 Max = glm::vec3{ std::numeric_limits<float>::lowest() };

		AABB(const aiVector3D& min, const aiVector3D& max)
			: Min(min.x, min.y, min.z), Max(max.x, max.y, max.z) {
		}
		
		AABB(const glm::vec3& min, const glm::vec3& max)
			: Min(min.x, min.y, min.z), Max(max.x, max.y, max.z) {
		}

		AABB(const aiAABB& aabb)
			: Min(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z), Max(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z) {
		}
		AABB() = default;

		inline void Grow(const glm::vec3& p) {
			Min = glm::min(Min, p);
			Max = glm::max(Max, p);
		}

		inline void Grow(const AABB& box) {
			Min = glm::min(Min, box.Min);
			Max = glm::max(Max, box.Max);
		}

		inline float Area() {
			glm::vec3 e = Max - Min;
			return e.x * e.y + e.y * e.z + e.z * e.x;
		}

		inline void operator=(const aiAABB& aabb) {
			Min.x = aabb.mMin.x;
			Min.y = aabb.mMin.y;
			Min.z = aabb.mMin.z;

			Max.x = aabb.mMax.x;
			Max.y = aabb.mMax.y;
			Max.z = aabb.mMax.z;
		}

		inline float operator[](uint32_t index) {
			return index == 0 ? Min.x : (index == 1 ? Min.y : Min.z);
		}
	};

	/*
		* if leaf, number of triangle indices. Else array index of left child.
		* if leaf, offset into indices buffer. Else index of right child.
		* bool for inner node or leaf node
	*/
	struct BVHNode {
		AABB AABB;
		size_t LeftOrCount, RightOrOffset;
		bool IsLeaf = false;
	};
}