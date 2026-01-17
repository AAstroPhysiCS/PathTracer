workspace "PathTracer"
    architecture "x64"
    startproject "PathTracerEditor"

    configurations { "Debug", "Release" }

    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
    objdir "bin-int/%{cfg.buildcfg}/%{prj.name}"

include "PathTracerEngine"
include "PathTracerEditor"