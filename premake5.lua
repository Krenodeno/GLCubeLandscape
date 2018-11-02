workspace "TPs"
	configurations {"debug", "release"}
	platforms "x64"
	architecture "x64"

	location "build"
	debugdir "./"
	targetdir "bin"

	local gKitDir = "/home/thomas/Libraries/gkit2light/"
	local gKitFiles = { gKitDir .. "src/gKit/*.cpp",  gKitDir .. "src/gKit/*.h" }

	includedirs (gKitDir .. "src/gKit")

	language "C++"
	cppdialect "C++11"

	filter "configurations:debug"
		symbols "on"

	filter "configurations:release"
		optimize "speed"

	filter "system:linux"
		buildoptions { "-W -Wall -Wextra -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable", "-pipe" }
		links { "GLEW", "SDL2", "SDL2_image", "GL" }

	filter { "system:linux", "configurations:release" }
		buildoptions { "-mtune=native -march=native" }
		buildoptions { "-flto" }
		linkoptions { "-flto" }
		buildoptions { "-fopenmp" }
		linkoptions { "-fopenmp" }

project "gKit"
	kind "StaticLib"
	files (gKitFiles)

project "TP1"
	kind "ConsoleApp"
	files {"TP1/**.hpp", "TP1/**.cpp"}

	links "gKit"

project "TP2"
	kind "ConsoleApp"
	files {"TP2/**.hpp", "TP2/**.cpp"}
	--buildoptions { "-fno-omit-frame-pointer" }
	--buildoptions { "-fsanitize=undefined,address" }
	--linkoptions { "-fsanitize=undefined,address" }

	links "gKit"
