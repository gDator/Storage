workspace "PROJECTS"
	configurations {"Debug", "Release"}
	platforms {"Win64", "Static", "Shared", "Linux"}
	toolset ("clang")
project "STORAGE"
	location "./"
	kind "ConsoleApp"
	language "C++"
	targetdir "./" --bin/%{cfg.buildcfg}"
	targetname "storage"
	files {"**.hpp", "**.h", "**.c", "**.cpp", "**.cc", "**.idl"}
	includedirs{"include",
	"./vendor/imgui/include",
	"./vendor/cpr/include",
	"./vendor/Logger",
	"./vendor/stb",
	"D:/DEV/boost_1_82_0",
	"vendor/GL",
	"vendor/sqlite",
	"vendor",
	"vendor/simpleini"}
	removefiles{}
	
	linkoptions{}
	libdirs{"lib", 
	"D:/DEV/boost_1_82_0/stage/lib"}
	defines("ENGLISH")
	--libdirs{os.findlib("OpenGL")}
	stl ("libc++")
	filter "files:**.cpp"
		buildoptions {"-std=c++17"}
	filter "configurations:Debug"
		defines {"DEBUG"}
		symbols "On"
		buildoptions{"-g3", "-fdata-sections", "-ffunction-sections"}
	filter "configurations:Release"
		defines {"DEBUG"} -- for logging
		optimize "On"
		linkoptions {"-fno-exceptions", "-fno-rtti", "-fPIC"}
		buildoptions {"-fdata-sections", "-ffunction-sections", "-m64", "-Wall", "-Wextra", "-pthread"}
		kind "WindowedApp"
	filter { "platforms:Linux" }
    	system "Linux"
    	architecture "x86_64"
		links{"glew32s",
			"pthread",
			"sqlite3",
			"SQLiteCpp",
			"glfw3",
			"boost_log",
			"boost_thread",
			"boost_regex",
			"boost_filesystem",	
			"boost_log_setup",
			"boost_atomic",
			"boost_chrono"}
	filter { "platforms:Win64" }
    	system "Windows"
    	architecture "x86_64"
		links{"glew32s",
			"opengl32",
			"ole32",
			"oleaut32",
			"imm32",
			"version",
			"iconv",
			"pthread",
			"z",
			"sqlite3",
			"SQLiteCpp",
			"comdlg32",
			"gdi32",
			"glfw3",
			"boost_log-mt-x64",
			"boost_thread-mt-x64",
			"boost_regex-mt-x64",
			"boost_filesystem-mt-x64",	
			"boost_log_setup-mt-x64",
			"boost_atomic-mt-x64",
			"boost_chrono-mt-x64",
			"curl",
			"cpr", 
			"shell32"}
	filter { "platforms:Static" }
    	kind "StaticLib"
	filter { "platforms:DLL" }
    	kind "SharedLib"
    	defines { "DLL_EXPORTS" }
