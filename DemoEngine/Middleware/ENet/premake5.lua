project "ENet"
	kind "staticlib"
	language "C"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"include/**.h",
		"*.c" 
	}
	
	includedirs
	{
		"include/"
	}
	
	links {"ws2_32", "winmm"}
	
	filter "system:windows"
		buildoptions {"-std=c11"}
		systemversion "latest"
		staticruntime "On"
		
		files
		{
			"win32.c"
		}
		
		defines
		{
			"ws2_32"
		}
		
		filter {"system:windows" , "configurations:Release"}