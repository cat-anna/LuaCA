
local Libs_common	= {	"freeimage", "boost_thread-mgw47-mt-1_54","boost_system-mgw47-mt-1_54", 
						"boost_date_time-mgw47-mt-1_54", "boost_chrono-mgw47-mt-1_54",
						"glfw3", "lua51jit", "pugi-1.2_char" } 
	
local Libs_lin		= { "GL", "GLU", "GLEW", "X11", "Xxf86vm", "Xrandr", "Xi", "rt" }
local Libs_win		= { "opengl32", "glu32", "gdi32", "glew32" }

-- local LibDirs 		= { }
local IncDirs 		= {	"./", "src/**" }
local SrcDirs 		= {	"./*", "src/**" }

solution "MW"
	configurations { "Debug", "Release" }
	language "C++"
	
	links(Libs_common)
	-- libdirs(LibDirs)
	includedirs(IncDirs)
	
	configuration "Debug"
        defines { "DEBUG" }	
		flags { "Symbols", "ExtraWarnings" }
		buildoptions { "-Wno-reorder" }
	configuration "Release"
		defines { "NDEBUG" }
		flags { "OptimizeSpeed", "EnableSSE2" }   	-- 

	configuration "windows"
		links(Libs_win)
	configuration "linux"
		links(Libs_lin)
	
	configuration "gmake"
		buildoptions { "-std=c++0x" }
	
	project "Luaca"
		kind "ConsoleApp"
		location "bin"
		files(SrcDirs)
		
		--configuration "Release"
			--postbuildcommands { "copy Luaca.exe rLuaca.exe"} 
		