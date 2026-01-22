# GPU-based OpenGL Path Tracer

This repository contains a **GPU-based path tracer** implemented using **OpenGL 4.6 compute shaders**.  

<div align="center">
<img src="https://github.com/AAstroPhysiCS/PathTracer/blob/main/Github/bistro2.png" width="100%" alt="Bistro" />
</div>

The project was developed as part of a university graphics course. The primary motivation was to implement path tracing on older GPUs without dedicated ray tracing cores. Given that OpenGL does not provide native GPU-accelerated ray tracing, this project demonstrates a fully shader-based approach. The renderer evaluates the rendering equation stochastically and progressively converges to physically plausible images through frame accumulation.

Included in this repository are several models located at:

```
PathTracer/PathTracerEditor/Assets/Models
```

You can experiment with these models or create your own scenes. (See the screenshots for some examples)



- **Physically Based Path Tracing**
  - GGX Microfacet BRDF with Smith shadowing-masking
  - Metallic–roughness workflow (glTF-style)
  - Energy-conserving diffuse and specular shading
  - Bindless texture support (GL_ARB_bindless_texture) for zero-overhead material sampling
  - Multiple Importance Sampling (MIS)
- **Samplers**
  - Cosine-weighted hemisphere sampling for diffuse lobes
  - VNDF (Visible Normal Distribution Function) sampling for specular lobes
- **Direct Lighting**
  - Directional light (sun) with Next Event Estimation
  - Shadow ray visibility testing
- **Environment Lighting**
  - Simple gradient-based sky model
- **Bounding Volume Hierarchy (BVH) using Binning**
  - CPU-side generation
  - GPU-side traversal

## Building the Project
First of all, these requirements must be fullfilled in order to run this PathTracer.
- GPU with support for **OpenGL 4.6**
- Support for:
  - `GL_ARB_bindless_texture`
  - `GL_NV_gpu_shader5` (or equivalent functionality)

Since this project already contains all the external dependencies, you can build it via Windows subsystems with Premake by running the Script **Generate.bat**. 

```bash
git clone https://github.com/AAstroPhysiCS/PathTracer.git
cd PathTracer
./Generate.bat
```

## Future Improvements
- **Wavefront path tracing**
- **Atmospheric Rendering**  [A Scalable and Production Ready Sky and Atmosphere Rendering Technique](https://sebh.github.io/publications/egsr2020.pdf)
- **GPU-side BVH construction**
- **Advanced Denoising** (**Optix** or **SVGF (Spatiotemporal Variance-Guided Filtering)**)
- **Subsurface scattering**
- **Motion blur and depth of field**

## Screenshots

<div align="center">
  <table style="border-collapse: collapse; border: none;">
    <tr>
      <td align="center" style="border: none;">
        <img src="https://github.com/AAstroPhysiCS/PathTracer/blob/main/Github/bistro1.png" width="33%" alt="Bistro" /><br>
        <b>Bistro</b><br>
      </td>
      <td align="center" style="border: none;">
        <img src="https://github.com/AAstroPhysiCS/PathTracer/blob/main/Github/salle_de_bain.png" width="33%" alt="Salle de Bain" /><br>
        <b>Salle de Bain</b><br>
      </td>
      <td align="center" style="border: none;">
        <img src="https://github.com/AAstroPhysiCS/PathTracer/blob/main/Github/sponza2.png" width="33%" alt="Sponza Atrium" /><br>
        <b>Sponza Atrium</b><br>
      </td>
    </tr>
  </table>
</div>

## References

1. Pharr, M., Jakob, W., & Humphreys, G. (2024).  
   [*Physically Based Rendering: From Theory to Implementation* (4rd ed.)](https://www.pbr-book.org/).  
   Morgan Kaufmann.

2. Walter, B., Marschner, S. R., Li, H., & Torrance, K. E. (2007).  
   [*Microfacet Models for Refraction through Rough Surfaces*](https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf).  
   Proceedings of the Eurographics Symposium on Rendering.

3. Heitz, E. (2014).  
   [*Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs*](https://jcgt.org/published/0003/02/03/).  
   Journal of Computer Graphics Techniques (JCGT), 3(2).

4. Heitz, E. (2018).  
   [*Sampling the GGX Distribution of Visible Normals*](https://jcgt.org/published/0007/04/01/).  
   Journal of Computer Graphics Techniques (JCGT), 7(4).

5. Veach, E., & Guibas, L. J. (1995).  
   [*Optimally Combining Sampling Techniques for Monte Carlo Rendering*](https://graphics.stanford.edu/papers/veach_thesis/).  
   Proceedings of SIGGRAPH 1995.

6. Narkowicz, K. (2015).  
   [*ACES Filmic Tone Mapping Curve*](https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/).  
   Self Shadow Blog.

7. Reed, J. (2016).  
   [*Hash Functions for GPU Rendering*](https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/).  
   Reedbeta Blog.

8. Auziaffe, M. (2024).  
   [*VNDF Importance Sampling for Isotropic GGX Distributions*](https://auzaiffe.wordpress.com/2024/04/15/vndf-importance-sampling-an-isotropic-distribution/).  
   Technical Blog Article.