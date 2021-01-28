-- Projects

project("Example")

	language("C")

	kind("ConsoleApp")
	systemversion("latest")

	-- Build flags
	filter("toolset:not msc*")
		buildoptions({ "-Wall", "-Wextra" })
	filter("toolset:msc*")
		buildoptions({ "-W3"})
	filter({})

	-- Common include dirs
	sysincludedirs({ "../include/", "common/glfw/include/", "common/" })

	files({"common/common.c", "common/common.h"})
	links({"glfw3", "sr_gui"})

	-- Libraries for each platform.
	filter("system:macosx")
		links({"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreVideo.framework", "AppKit.framework", "AVFoundation.framework", "CoreMedia.framework", "Accelerate.framework" })

	filter("system:windows")
		links({"opengl32", "comctl32", "mfplat", "mf", "mfuuid", "Mfreadwrite", "Shlwapi"})

	filter("system:linux")
		links({"GL", "X11", "Xi", "Xrandr", "Xxf86vm", "Xinerama", "Xcursor", "Xext", "Xrender", "Xfixes", "xcb", "Xau", "Xdmcp", "rt", "m", "pthread", "dl"})
	
		
	filter({})

	files({ "main.c" })


include("example/common/glfw/premake5.lua")

