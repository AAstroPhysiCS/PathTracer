project "PathTracerEngine"
    staticruntime "Off"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    files {
        "Source/**.h",
        "Source/**.hpp",
        "Source/**.cpp",

        "Libraries/glad/src/glad.c",
        "Libraries/DDSLoader/src/dds.c",

        "Libraries/imgui/include/**.h",
        "Libraries/imgui/include/**.cpp",

        "Libraries/imgui/include/backends/**.h",
        "Libraries/imgui/include/backends/**.cpp",
    }

    includedirs {
        "Source",

        "Libraries/glad/include",
        "Libraries/glfw/include",
        "Libraries/glm/include",
        "Libraries/stb/include",
        "Libraries/assimp/include",
        "Libraries/imgui/include",
        "Libraries/DDSLoader/include",
    }

    filter "configurations:Debug"
        defines { "DEBUG", "GLFW_INCLUDE_NONE", "USE_STD_FILESYSTEM" }
        symbols "On"

        links {
            "Libraries/glfw/lib-vc2022/glfw3.lib",
            "Libraries/glm/bin/glm.lib",
            "Libraries/assimp/lib/x64/assimp-vc143-mt.lib",
        }

    filter "configurations:Release"
        defines { "NDEBUG", "GLFW_INCLUDE_NONE", "USE_STD_FILESYSTEM" }
        optimize "On"

        links {
            "Libraries/glfw/lib-vc2022/glfw3_mt.lib",
            "Libraries/glm/bin/glm.lib",
            "Libraries/assimp/lib/x64/assimp-vc143-mt.lib",
        }
