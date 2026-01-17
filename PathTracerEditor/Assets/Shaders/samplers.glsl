#include "commons.glsl"

/*
    * From: https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
*/
uint pcg_hash(uint i) {
    uint state = i * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float Rand(inout uint seed) {
    seed = pcg_hash(seed);
    return float(seed) / float(0xffffffffu);
}

/*
    * Since its better than uniform distribution
*/
vec3 CosineWeightedHemisphereSampling(vec3 n, inout uint seed) {
    float r1 = Rand(seed);
    float r2 = Rand(seed);

    float phi = 2.0 * PI * r1;
    float cosTheta = sqrt(1.0 - r2);
    float sinTheta = sqrt(r2);

    vec3 tangent = normalize(
        abs(n.x) > 0.1 ? cross(vec3(0,1,0), n) : cross(vec3(1,0,0), n)
    );
    vec3 bitangent = cross(n, tangent);

    return normalize(
        tangent * cos(phi) * sinTheta +
        bitangent * sin(phi) * sinTheta +
        n * cosTheta
    );
}

vec3 GGX_Sample(float roughness, vec3 N, vec3 V, inout uint seed) {
    float u1 = Rand(seed);
    float u2 = Rand(seed);
    float a = roughness*roughness;

    float phi = 2.0*PI*u2;
    float cosTheta = sqrt((1.0 - u1)/(1.0 + (a*a - 1.0)*u1));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // Tangent space basis
    vec3 T, B;
    if(abs(N.z)<0.999) T = normalize(cross(N, vec3(0,0,1)));
    else T = vec3(1,0,0);
    B = cross(N,T);

    // Microfacet normal H in world space
    vec3 H = normalize(T*sinTheta*cos(phi) + B*sinTheta*sin(phi) + N*cosTheta);

    // Reflect view vector around H
    vec3 L = normalize(2.0*dot(V,H)*H - V);

    if(dot(L,N)<=0.0) L = N; // Ensure in hemisphere
    return L;
}
