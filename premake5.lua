project "Minimal"
	kind "StaticLib"
	language "C"
	
	targetdir ("build/bin/" .. output_dir .. "/%{prj.name}")
	objdir ("build/bin-int/" .. output_dir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.c"
	}

	includedirs
	{
		"src",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"