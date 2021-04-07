workspace "Minimal"
	architecture "x64"
	startproject "Minimal"

	configurations
	{
		"Debug",
		"OptimizedDebug",
		"Release"
	}

outputDir = "%{cfg.buildcfg}"   

project "Minimal"
	location "Minimal"
	kind "ConsoleApp"
	language "C"
	cdialect "C99"
	staticruntime "On"
	
	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

	files
	{
		--Config
		"%{prj.name}/config.ini",
		--Source
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c"
	}

	links
	{
		"opengl32"
	}

	includedirs
	{
		"%{prj.name}/src",
	}

	filter "system:windows"
		systemversion "latest"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:OptimizedDebug"
		runtime "Debug"
		symbols "On"
		optimize "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"