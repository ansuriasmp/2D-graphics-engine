workspace "DemoEngine" 
	architecture "x64"
	startproject "DemoEngine"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include directories relative to root folider (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "DemoEngine/Middleware/GLFW/include"
IncludeDir["Glad"] = "DemoEngine/Middleware/Glad/include"
IncludeDir["spdlog"] = "DemoEngine/Middleware/spdlog/include"
IncludeDir["glm"] = "DemoEngine/Middleware/glm"
IncludeDir["IMGUI"] = "DemoEngine/Middleware/IMGUI"
IncludeDir["enTT"] = "DemoEngine/Middleware/enTT/single_include/entt"
IncludeDir["IMGUIZMO"] = "DemoEngine/Middleware/ImGuizmo"
IncludeDir["YAMLCPP"] = "DemoEngine/Middleware/YAML-CPP/include"
IncludeDir["Box2D"] = "DemoEngine/Middleware/Box2D/include"
IncludeDir["SoLoud"] = "DemoEngine/Middleware/SoLoud/include"
IncludeDir["ENet"] = "DemoEngine/Middleware/ENet/include"

--includes the premake5.lua file for middleware, effectively bringing it into this premake file 
include "DemoEngine/Middleware/GLFW"
include "DemoEngine/Middleware/Glad"
include "DemoEngine/Middleware/IMGUI"
include "DemoEngine/Middleware/YAML-CPP"
include "DemoEngine/Middleware/Box2D"
include "DemoEngine/Middleware/SoLoud"
include "DemoEngine/Middleware/ENet"

project "DemoEngine"
	location "DemoEngine" 
	kind "ConsoleApp" 
	language "C++"
	staticruntime "on"
	cppdialect "C++17"

	targetdir ("bin/"..outputdir.."/%{prj.name}") 
	objdir ("bin-int/"..outputdir.."/%{prj.name}")

	pchheader "DemoEngine_PCH.h"
	pchsource "%{prj.name}/src/DemoEngine_PCH.cpp"

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{IncludeDir.IMGUIZMO}/**.h",
			"%{IncludeDir.IMGUIZMO}/**.cpp",
		}

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"YAML_CPP_STATIC_DEFINE"
		}	

		includedirs
		{
			"%{prj.name}/src/",
			"%{prj.name}/Middleware",
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.Glad}",
			"%{IncludeDir.spdlog}",
			"%{IncludeDir.glm}",
			"%{IncludeDir.IMGUI}",
			"%{IncludeDir.enTT}",
			"%{IncludeDir.IMGUIZMO}",
			"%{IncludeDir.YAMLCPP}",
			"%{IncludeDir.Box2D}",
			"%{IncludeDir.SoLoud}",
			"%{IncludeDir.ENet}",
		}

		links
		{
			"GLFW",
			"Glad",
			"IMGUI",
			"YAML-CPP",
			"Box2D",
			"SoLoud",
			"ENet"
		}

		filter "files:DemoEngine/Middleware/ImGuizmo/**.cpp"
			flags{"NoPCH"}

		filter "system:windows"
			systemversion "latest"
			buildoptions {"/utf-8"}
			
			defines
			{
			--this is required to avoid errors with the OpenGL header already being included --being displayed in visual studio.
			--this explicitly prevents the GLFW header from including the OpenGL header 
				"GLFW_INCLUDE_NONE"
			}

		
		filter "configurations:Debug" 
			staticruntime "off"
			runtime "Debug" 
			symbols "On"

		filter "configurations: Release" 
			staticruntime "off"
			runtime "Release" 
			optimize "on"

		filter "configurations:Dist"
			runtime "Release"
			optimize "on"
			symbols "Off"