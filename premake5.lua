workspace "PROJECTS"
	configurations {"Debug", "Release"}
	platforms {"Win64", "Static", "Shared"}
	toolset ("clang")
project "STORAGE"
	location "./"
	kind "ConsoleApp"
	language "C++"
	targetdir "./" --bin/%{cfg.buildcfg}"
	targetname "storage"
	files {"**.hpp", "**.h", "**.c", "**.cpp", "**.cc", "**.idl"}
	includedirs{"include",
	"./vendor/SFML",
	"./vendor/imgui/include",
	"D:/DEV/boost_1_70_0",
	"vendor/GL",
	"vendor/sqlite",
	"vendor",
	"vendor/simpleini"}
	removefiles{}
	links{"glew32s",
	"opengl32",
	"sfml-graphics",
	"sfml-window",
	"sfml-system",
	"ole32",
	"oleaut32",
	"imm32",
	"version",
	"iconv",
	"pthread",
	"z",
	"boost_date_time-clang15-mt-s-x64-1_70",
	"boost_log-clang15-mt-s-x64-1_70",
	"boost_thread-clang15-mt-s-x64-1_70",
	"boost_regex-clang15-mt-s-x64-1_70",
	"boost_filesystem-clang15-mt-s-x64-1_70",
	"boost_log_setup-clang15-mt-s-x64-1_70",
	"boost_atomic-clang15-mt-s-x64-1_70",
	"boost_chrono-clang15-mt-s-x64-1_70",
	"sqlite3",
	"SQLiteCpp",
	"comdlg32"}
	linkoptions{}
	libdirs{"lib", 
	"D:/DEV/boost_1_70_0/static_stage/lib"}
	--libdirs{os.findlib("OpenGL")}
	stl ("libc++")
	filter "files:**.cpp"
		buildoptions {"-std=c++17"}
	filter "configurations:Debug"
		defines {"DEBUG"}
		symbols "On"
		buildoptions{"-g3", "-fdata-sections", "-ffunction-sections"}
	filter "configurations:Release"
		defines {}
		optimize "On"
		linkoptions {"-fno-exceptions", "-fno-rtti", "-fPIC"}
		buildoptions {"-fdata-sections", "-ffunction-sections", "-m64", "-Wall", "-Wextra", "-pthread"}
		kind "WindowedApp"
	filter { "platforms:Win64" }
    	system "Windows"
    	architecture "x86_64"
	filter { "platforms:Static" }
    	kind "StaticLib"
	filter { "platforms:DLL" }
    	kind "SharedLib"
    	defines { "DLL_EXPORTS" }
