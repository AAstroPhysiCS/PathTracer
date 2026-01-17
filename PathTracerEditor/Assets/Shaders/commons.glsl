#define PI 3.14159265359

struct Ray {
	vec3 Direction;
	vec3 Origin;
    vec3 InvDirection;
    ivec3 Sign;
};

struct BVHNode {
    vec4 MinLeftOrCount, MaxRightOrOffset;
    int IsLeaf;
};

struct Entity {
    mat4 Transform;
    mat4 InverseTransposeTransform;
};

struct Material {
    vec4 DiffuseColor;
    vec4 EmissiveColor;

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
    
struct MaterialData {
    vec4 Albedo;
    vec4 Emission;
    vec3 Normal;
    float Roughness;
    float Metallic;
    float AO;
};

struct Triangle {
    vec4 V0;
    vec4 V1;
    vec4 V2;

    vec4 N0;
    vec4 N1;
    vec4 N2;
    
    vec4 T0;
    vec4 T1;
    vec4 T2;

    vec4 UV0;
    vec4 UV1;
    vec4 UV2;

    uvec2 ID; //x = MaterialID, y = EntityID
};

/*
    * From: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm and 
    * https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection.html
    * modified it to my own needs
*/
bool RayTriangleIntersect(in vec3 orig, in vec3 dir, in vec3 v0, in vec3 v1, in vec3 v2, out float t, out vec3 normal, out float u, out float v) {
    const float EPSILON = 1e-9;

    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    vec3 pvec = cross(dir, edge2);
    float det = dot(edge1, pvec);

    // Ray is parallel to the triangle
    if (abs(det) < EPSILON)
        return false;

    float invDet = 1.0 / det;

    vec3 tvec = orig - v0;
    u = dot(tvec, pvec) * invDet;

    if (u < 0.0 || u > 1.0)
        return false;

    vec3 qvec = cross(tvec, edge1);
    v = dot(dir, qvec) * invDet;

    if (v < 0.0 || (u + v) > 1.0)
        return false;

    // Compute t to find intersection point along the ray
    t = dot(edge2, qvec) * invDet;

    // Intersection is behind the ray origin
    if (t <= EPSILON)
        return false;

    // Triangle normal (flat shading)
    normal = normalize(cross(edge1, edge2));

    return true;
}

bool RayTriangleIntersect(in vec3 orig, in vec3 dir, in Triangle triangle, out float t, out vec3 normal, out float u, out float v) {
    return RayTriangleIntersect(orig, dir, triangle.V0.xyz, triangle.V1.xyz, triangle.V2.xyz, t, normal, u, v);
}

bool RayTriangleIntersect(in Ray ray, in Triangle triangle, out float t, out vec3 normal, out float u, out float v) {
    return RayTriangleIntersect(ray.Origin, ray.Direction, triangle, t, normal, u, v);
}

bool RayBoxIntersection(in Ray r, in BVHNode node, out float tNear, out float tFar) {
    vec3 minCorner = node.MinLeftOrCount.xyz;
    vec3 maxCorner = node.MaxRightOrOffset.xyz;

    vec3 invDir = 1.0 / r.Direction;

    vec3 t0s = (minCorner - r.Origin) * invDir;
    vec3 t1s = (maxCorner - r.Origin) * invDir;

    vec3 tmin = min(t0s, t1s);
    vec3 tmax = max(t0s, t1s);

    tNear = max(max(tmin.x, tmin.y), tmin.z);
    tFar  = min(min(tmax.x, tmax.y), tmax.z);

    return tFar >= max(tNear, 0.0);
}
