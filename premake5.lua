workspace "TPs"
	configurations {"debug", "release"}
	platforms "x64"
	architecture "x64"

	location "build"
	debugdir "./"

	local gKitDir = "/home/thomas/Libraries/gkit2light/"
	local gKitFiles = { gKitDir .. "src/gKit/*.cpp",  gKitDir .. "src/gKit/*.h" }

	includedirs (gKitDir .. "src/gKit")

	cppdialect "C++11"

configuration "debug"
	symbols "on"

configuration "release"
	optimize "speed"

configuration "linux"
	buildoptions { "-mtune=native -march=native" }
	buildoptions { "-W -Wall -Wextra -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable", "-pipe" }
	buildoptions { "-flto"}
	linkoptions { "-flto"}
	links { "GLEW", "SDL2", "SDL2_image", "GL" }

configuration { "linux", "release" }
	buildoptions { "-fopenmp" }
	linkoptions { "-fopenmp" }

project "gKit"
	language "C++"
	kind "StaticLib"
	targetdir "bin"
	files (gKitFiles)

project "TP1"
	language "C++"
	kind "ConsoleApp"
	targetdir "bin"
	--files (gKitFiles)
	files {"TP1/**.hpp", "TP1/**.cpp"}

	links "gKit"
