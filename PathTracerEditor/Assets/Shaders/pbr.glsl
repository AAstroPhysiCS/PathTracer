// GGX NDF
float D_GGX(float alpha, vec3 N, vec3 H) {
    float NdotH = max(dot(N,H), 0.0);
    float a2 = alpha * alpha;
    float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

// Schlick Fresnel
vec3 Fresnel_Schlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Geometry (Smith) function
float G_Smith(float alpha, vec3 N, vec3 V, vec3 L) {
    float k = (alpha + 1.0) * (alpha + 1.0) / 8.0;
    float NdotV = max(dot(N,V), 0.0);
    float NdotL = max(dot(N,L), 0.0);
    return NdotV / (NdotV*(1.0 - k) + k) * NdotL / (NdotL * (1.0 - k) + k);
}

// Full microfacet BRDF
vec3 EvaluatePBR(MaterialData mat, vec3 V, vec3 L, vec3 N) {
    vec3 H = normalize(V + L);
    float alpha = mat.Roughness * mat.Roughness;

    // Fresnel reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), mat.Albedo.rgb, mat.Metallic);

    // D, G, F
    float D = D_GGX(mat.Roughness, N, H);
    float G = G_Smith(mat.Roughness, N, V, L);
    vec3 F = Fresnel_Schlick(max(dot(H,V), 0.0), F0);

    vec3 spec = D * G * F / (4.0 * max(dot(N,V), 0.0) * max(dot(N,L), 0.0) + 1e-5);
    vec3 kD = (1.0 - F) * (1.0 - mat.Metallic);
    vec3 diffuse = kD * mat.Albedo.rgb / PI;

    return spec + diffuse;
}

// PDF for GGX importance sampling
float GGX_PDF(float alpha, vec3 N, vec3 V, vec3 L) {
    vec3 H = normalize(V + L);
    float D = D_GGX(alpha, N, H);
    float NdotH = max(dot(N,H), 0.0);
    float VdotH = max(dot(V,H), 0.0);
    return D * NdotH / (4.0 * VdotH + 1e-5);
}