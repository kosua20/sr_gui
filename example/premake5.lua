-- On Linux we have to query the dependencies for libsecret
if os.ishost("linux") then
	listing, code = os.outputof("pkg-config --libs libnotify gtk+-3.0")
	liballLibs = string.explode(string.gsub(listing, "-l", ""), " ")
end

-- Projects
function CommonExampleSetup()
	language("C")

	kind("ConsoleApp")
	systemversion("latest")

	filter("system:macosx")
		systemversion("10.12:latest")
	filter({})

	-- Build flags
	filter("toolset:not msc*")
		buildoptions({ "-Wall", "-Wextra" })
	filter("toolset:msc*")
		buildoptions({ "-W3"})
	filter({})

	-- Common include dirs
	externalincludedirs({ "../include/", "common/glfw/include/", "common/" })

	files({"common/common.c", "common/common.h"})
	links({"glfw3"})

	-- Libraries for each platform.
	filter("system:macosx")
		links({"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreVideo.framework", "AppKit.framework"})

	filter("system:windows")
		links({"opengl32", "User32", "Comdlg32", "Comctl32", "Shell32", "runtimeobject"})

	filter("system:linux")
		links({"GL", "X11", "Xi", "Xrandr", "Xxf86vm", "Xinerama", "Xcursor", "Xext", "Xrender", "Xfixes", "xcb", "Xau", "Xdmcp", "rt", "m", "pthread", "dl"})
	
		
	filter({})

end


project("Example")
	CommonExampleSetup()
	files({ "main.c" })
	links({"sr_gui"})
	filter("system:linux")
		links(liballLibs)

project("ExampleCLI")
	CommonExampleSetup()
	files({ "main.c" })
	links({"sr_gui_cli"})
	


include("example/common/glfw/premake5.lua")

