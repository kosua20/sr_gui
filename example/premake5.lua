-- Projects
function CommonExampleSetup()
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
	links({"glfw3"})

	-- Libraries for each platform.
	filter("system:macosx")
		links({"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreVideo.framework", "AppKit.framework", "AVFoundation.framework", "CoreMedia.framework", "Accelerate.framework" })

	filter("system:windows")
		links({"opengl32", "comctl32", "User32", "Comdlg32", "Comctl32"})

	filter("system:linux")
		links({"GL", "X11", "Xi", "Xrandr", "Xxf86vm", "Xinerama", "Xcursor", "Xext", "Xrender", "Xfixes", "xcb", "Xau", "Xdmcp", "rt", "m", "pthread", "dl"})
	
		
	filter({})

end


project("Example")
	CommonExampleSetup()
	files({ "main.c" })
	links({"sr_gui"})

project("ExampleCLI")
	CommonExampleSetup()
	files({ "main.c" })
	links({"sr_gui_cli"})
	


include("example/common/glfw/premake5.lua")

