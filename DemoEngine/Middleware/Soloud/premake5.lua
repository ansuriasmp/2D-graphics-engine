project "SoLoud"
    kind "StaticLib"
    language "C++"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    
    files {
        "src/**.cpp",
        "src/**.c",
        "include/**.h"
    }
    
    includedirs {
        "include"
    }
    defines { 
        "WITH_MINIAUDIO",  
        "WITH_WAV"
    }
    filter "system:windows"
    systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        symbols "on"
