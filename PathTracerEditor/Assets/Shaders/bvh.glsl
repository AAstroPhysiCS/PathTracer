/*
	* From: https://developer.nvidia.com/blog/thinking-parallel-part-ii-tree-traversal-gpu/
	* inspired from this nvidia blog. its unfortunately CUDA only, but luckily the theory behind is the same
*/

#define BVH_DEPTH 64

/*
    * if leaf, number of triangle indices. Else array index of left child
    * if leaf, offset into indices buffer. Else index of right child
    * bool for inner node or leaf node
*/

layout(std430, binding = 3) buffer BVH {
    BVHNode s_BVHNodes[];
};

void ApplyTransformation(inout Triangle tri) {
    mat4 transform = s_Entites[tri.ID.y].Transform;
    tri.V0 = transform * tri.V0;
    tri.V1 = transform * tri.V1;
    tri.V2 = transform * tri.V2;

    mat4 normalTransform = s_Entites[tri.ID.y].InverseTransposeTransform;
    tri.N0 = normalTransform * tri.N0;
    tri.N1 = normalTransform * tri.N1;
    tri.N2 = normalTransform * tri.N2;
}

void TraverseBVH(in Ray ray, out bool hit, out float closestT, out Triangle hitTri, out vec3 hitNormal, out float hitU, out float hitV) {
    uint stack[BVH_DEPTH];
    uint stackPtr = 0;

    hit = false;
    closestT = 1e30;

    stack[stackPtr++] = 0; // root index

    while (stackPtr > 0 && stackPtr < BVH_DEPTH) {  
        uint nodeIndex = stack[--stackPtr];
        BVHNode node = s_BVHNodes[nodeIndex];

        if (node.IsLeaf == 1) { 
            uint count = uint(node.MinLeftOrCount.w);
            uint offset = uint(node.MaxRightOrOffset.w);

            for (uint i = 0; i < count; i++) {
                Triangle tri = s_Triangles[offset + i];
                //ApplyTransformation(tri);
                vec3 normal;
                float t, u, v;
                if (RayTriangleIntersect(ray, tri, t, normal, u, v)) {
                    if (t < closestT) {
                        closestT = t;
                        hitNormal = normal;
                        hitTri = tri;
                        hitU = u;
                        hitV = v;
                        hit = true;
                    }
                }
            }

            continue;
        } else {
            uint leftChild = uint(node.MinLeftOrCount.w); 
            uint rightChild = uint(node.MaxRightOrOffset.w);

            float tNearL, tFarL;
            float tNearR, tFarR;

            bool hitL = RayBoxIntersection(ray, s_BVHNodes[leftChild], tNearL, tFarL);
            bool hitR = RayBoxIntersection(ray, s_BVHNodes[rightChild], tNearR, tFarR);

            /*
                * Dont know why, but this way on my machine (aka. with more if statements), it performs much better on the sponza atrium scene...
            */

            // Fast paths first
            if (hitL && !hitR) {
                if (tNearL <= closestT)
                    stack[stackPtr++] = leftChild;
            } else if (hitR && !hitL) {
                if (tNearR <= closestT)
                    stack[stackPtr++] = rightChild;
            } else if (hitL && hitR) {
                if (tNearL < tNearR) {
                    if (tNearR <= closestT) stack[stackPtr++] = rightChild;
                    if (tNearL <= closestT) stack[stackPtr++] = leftChild;
                } else {
                    if (tNearL <= closestT) stack[stackPtr++] = leftChild;
                    if (tNearR <= closestT) stack[stackPtr++] = rightChild;
                }
            }
        }
    }
}