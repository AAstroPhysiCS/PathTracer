project "PathTracerEditor"
    staticruntime "Off"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    links { "PathTracerEngine" }

    files {
      "Source/**.h",
      "Source/**.hpp",
      "Source/**.cpp",
    }

    includedirs {
      "Source",

      "../PathTracerEngine/Source",
      "../PathTracerEngine/Libraries/glm/include",
      "../PathTracerEngine/Libraries/glfw/include",
      "../PathTracerEngine/Libraries/assimp/include",
      "../PathTracerEngine/Libraries/imgui/include",
      "../PathTracerEngine/Libraries/DDSLoader/include",
    }

    postbuildcommands {
      "{COPYFILE} %{wks.location}/PathTracerEngine/Libraries/assimp/bin/*.dll %{cfg.targetdir}",
      "{COPYFILE} %{wks.location}/PathTracerEditor/imgui.ini %{cfg.targetdir}",
      "{COPYDIR} %{wks.location}/PathTracerEditor/Assets %{cfg.targetdir}/Assets"
    }

    dependson { "PathTracerEngine" }
    
    filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"