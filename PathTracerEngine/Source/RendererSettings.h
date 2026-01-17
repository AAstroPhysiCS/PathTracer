#pragma once
#include <cstdint>
namespace PathTracer {

    struct RendererSettings {
        int32_t MaxBounces = 4;
        int32_t SamplesPerFrame = 1;
        int32_t MaxSamples = 1024;

        bool Accumulate = true;
        bool UseMIS = true;
        bool EnableDenoiser = false;

        bool ShowBVH = false;
        bool ShowRays = false;

        float Exposure = 1.0f;
        float Gamma = 2.2f;
    };

    struct RendererStats {
        int32_t SamplesAccumulated = 0;
        float FrameTimeMS = 0.0f;
        float GPUMemoryMB = 0.0f;
    };
}