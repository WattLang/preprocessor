Is64bit  = false
UseClang = true

workspace "WotScript"
    language "C++"
    cppdialect "C++17"

    configurations { "Debug", "Release" }

    filter { "configurations:Debug" }
        optimize "Off"
        symbols "On"
    filter { "configurations:Release" }
        optimize "On"
        symbols "Off"
    filter { }

    if Is64bit then

        location "build64"

        architecture "x86_64"

        targetdir ("build64/bin/%{prj.name}/%{cfg.longname}")

        objdir ("build64/obj/%{prj.name}/%{cfg.longname}")
    else 

        location "build32"

        architecture "x86"

        targetdir ("build32/bin/%{prj.name}/%{cfg.longname}")

        objdir ("build32/obj/%{prj.name}/%{cfg.longname}")
    end

    if UseClang then
        toolset "clang"
    end

project "Scanner-Preprocessor"

    kind "ConsoleApp"

    files { "src/**.hpp", "src/**.cpp" }

    includedirs "./lib/module"

    filter "configurations:Debug"
        defines { "DEBUG" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        
    filter { }