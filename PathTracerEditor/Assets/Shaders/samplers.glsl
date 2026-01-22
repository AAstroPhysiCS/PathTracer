#include "commons.glsl"
#include "pbr.glsl"

struct SampleResult {
    vec3 Direction;
    vec3 BRDF;
    float PDF;
};

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

/*
    * Since its better than CosineWeightedHemisphereSampling
*/
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

/*
    * From: https://gist.github.com/jdupuy/4c6e782b62c92b9cb3d13fbb0a5bd7a0#file-samplevndf_ggx-cpp-L51
    * Since its better than GGX_Sample
*/
vec3 SampleVndf_GGX(uint seed, vec3 wi, float alpha, vec3 n) {
    vec2 u = vec2(Rand(seed), Rand(seed));

    // decompose the vector in parallel and perpendicular components
    vec3 wi_z = n * dot(wi, n);
    vec3 wi_xy = wi - wi_z;
    // warp to the hemisphere configuration
    vec3 wiStd = normalize(wi_z - alpha * wi_xy);
    // sample a spherical cap in (-wiStd.z, 1]
    float wiStd_z = dot(wiStd, n);
    float phi = (2.0f * u.x - 1.0f) * PI;
    float z = (1.0f - u.y) * (1.0f + wiStd_z) - wiStd_z;
    float sinTheta = sqrt(clamp(1.0f - z * z, 0.0f, 1.0f));
    float x = sinTheta * cos(phi);
    float y = sinTheta * sin(phi);
    vec3 cStd = vec3(x, y, z);
    // reflect sample to align with normal
    vec3 up = vec3(0, 0, 1);
    vec3 wr = n + up;
    vec3 c = dot(wr, cStd) * wr / wr.z - cStd;
    // compute halfway direction as standard normal
    vec3 wmStd = c + wiStd;
    vec3 wmStd_z = n * dot(n, wmStd);
    vec3 wmStd_xy = wmStd_z - wmStd;
    // warp back to the ellipsoid configuration
    vec3 wm = normalize(wmStd_z + alpha * wmStd_xy);
    // return final normal
    return wm;
}

/*
    From: https://auzaiffe.wordpress.com/2024/04/15/vndf-importance-sampling-an-isotropic-distribution/
*/
float pdf_vndf_isotropic(vec3 wo, vec3 wi, float alpha, vec3 n) {
    float alphaSquare = alpha * alpha;
    vec3 wm = normalize(wo + wi);
    float zm = dot(wm, n);
    float zi = dot(wi, n);
    float nrm = inversesqrt((zi * zi) * (1.0f - alphaSquare) + alphaSquare);
    float sigmaStd = (zi * nrm) * 0.5f + 0.5f;
    float sigmaI = sigmaStd / nrm;
    float nrmN = (zm * zm) * (alphaSquare - 1.0f) + 1.0f;
    return alphaSquare / (PI * 4.0f * nrmN * nrmN * sigmaI);
}

SampleResult SampleDiffuse(MaterialData mat, vec3 N, inout uint seed) {
    SampleResult s;
    s.Direction = CosineWeightedHemisphereSampling(N, seed);

    float NdotL = max(dot(N, s.Direction), 0.0);
    s.PDF = NdotL / PI;
    s.BRDF = mat.Albedo.rgb / PI;
    return s;
}

SampleResult SampleSpecular(MaterialData mat, vec3 V, vec3 N, inout uint seed) {
    SampleResult s;
    s.Direction = SampleVndf_GGX(seed, V, mat.Roughness, N);

    float NdotL = max(dot(N, s.Direction), 0.0);
    if (NdotL <= 0.0) {
        s.PDF = 0.0;
        s.BRDF = vec3(0.0);
        return s;
    }

    s.PDF = pdf_vndf_isotropic(s.Direction, V, mat.Roughness, N);
    s.BRDF = EvaluatePBR(mat, V, s.Direction, N);
    return s;
}

float Luminance(vec3 c) {
    return dot(c, vec3(0.2126, 0.7152, 0.0722));
}

SampleResult SampleBSDF(MaterialData mat, vec3 V, vec3 N, inout uint seed, out float lobePdf, out float combinedPdf) {
    SampleResult s;
    vec3 F0 = mix(vec3(0.04), mat.Albedo.rgb, mat.Metallic);
    float specProb = clamp(Luminance(F0), 0.05, 0.95);

    if (Rand(seed) < specProb) {
        lobePdf = specProb;
        s = SampleSpecular(mat, V, N, seed);
    } else {
        lobePdf = 1.0 - specProb;
        s = SampleDiffuse(mat, N, seed);
    }

    // Compute combined PDF for MIS
    float pdfSpec = pdf_vndf_isotropic(V, s.Direction, mat.Roughness, N);
    float NdotL = max(dot(N, s.Direction), 0.0);
    float pdfDiff = NdotL / PI;
    combinedPdf = specProb * pdfSpec + (1.0 - specProb) * pdfDiff;

    return s;
}

float PowerHeuristic(float a, float b) {
    float a2 = a * a;
    float b2 = b * b;
    return a2 / (a2 + b2);
}

/*
    In the future, maybe sky rendering?
    this looks cool: https://www.shadertoy.com/view/slSXRW
*/
vec3 EvaluateEnvironment(vec3 dir) {
    // Simple sky gradient
    float t = 0.5 * (dir.y + 1.0);
    return mix(vec3(0.7, 0.8, 1.0), vec3(0.2, 0.4, 0.8), t);
}

float EnvironmentPDF() {
    return 1.0 / (4.0 * PI);
}

vec3 SampleEnvironment(inout uint seed, out float pdf) {
    float u1 = Rand(seed);
    float u2 = Rand(seed);

    float z = 1.0 - 2.0 * u1;
    float r = sqrt(max(0.0, 1.0 - z * z));
    float phi = 2.0 * PI * u2;

    pdf = EnvironmentPDF();
    return vec3(r * cos(phi), z, r * sin(phi));
}