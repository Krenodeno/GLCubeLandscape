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

	cppdialect "C++11"

configuration "debug"
	symbols "on"

configuration "release"
	optimize "speed"

configuration "linux"
	buildoptions { "-W -Wall -Wextra -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable", "-pipe" }
	links { "GLEW", "SDL2", "SDL2_image", "GL" }

configuration { "linux", "release" }
	buildoptions { "-mtune=native -march=native" }
	buildoptions { "-flto" }
	linkoptions { "-flto" }
	buildoptions { "-fopenmp" }
	linkoptions { "-fopenmp" }

project "gKit"
	language "C++"
	kind "StaticLib"
	files (gKitFiles)

project "TP1"
	language "C++"
	kind "ConsoleApp"
	--files (gKitFiles)
	files {"TP1/**.hpp", "TP1/**.cpp"}

	links "gKit"

project "TP2"
	language "C++"
	kind "ConsoleApp"
	--files (gKitFiles)
	files {"TP2/**.hpp", "TP2/**.cpp"}
	--buildoptions { "-fno-omit-frame-pointer" }
	--buildoptions { "-fsanitize=undefined,address" }
	--linkoptions { "-fsanitize=undefined,address" }

	links "gKit"
