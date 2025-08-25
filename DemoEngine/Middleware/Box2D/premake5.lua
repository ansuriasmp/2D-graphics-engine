project "Box2D"
	kind "StaticLib"
	language "C"
	cdialect "C17"
	staticruntime "on"


	targetdir ("bin/" .. "%{cfg.buildcfg}/%{cfg.system}/%{prj.name}")
	objdir ("bin-int/" ..  "%{cfg.buildcfg}/%{cfg.system}/%{prj.name}")

	files
	{
		"include/box2d/**.h",
		"src/**.h",
		"src/**.c",
		"src/**.cpp"
	}

	includedirs
	{
		"include",
		"include/box2d",
		"src"
	}

	defines { "BOX2D_ENABLE_SIMD" }

	-- Compiler-specific optimizations 
	filter "system:windows"
		systemversion "latest"
		buildoptions { "/arch:AVX2" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		defines { "BOX2D_DEBUG", "BOX2D_VALIDATE" }

	filter "configurations: Release"
		runtime "Release"
		optimize "on"
		defines { "BOX2D_RELEASE" }

	filter "configurations:Dist"
		runtime "Release"
		optimize "full"
		defines { "BOX2D_DIST" }