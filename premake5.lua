Is64bit = false

workspace "WotScript"
    language "C++"

    location "build"
    if(Is64bit) then
        location "build64"
    else 
        location "build32"
    end

    if(Is64bit) then
            architecture "x86_64"
    else
            architecture "x86"
    end

    configurations { "Debug", "Release" }

    filter { "configurations:Debug" }
            symbols "On"

    filter { "configurations:Release" }
            optimize "On"
    filter { }

    if(Is64bit) then
        targetdir ("build64/bin/%{prj.name}/%{cfg.longname}")

        objdir ("build64/obj/%{prj.name}/%{cfg.longname}")
    else 
        targetdir ("build32/bin/%{prj.name}/%{cfg.longname}")

        objdir ("build32/obj/%{prj.name}/%{cfg.longname}")
    end

project "Scanner-Preprocessor"

    kind "ConsoleApp"

    files { "src/**.hpp", "src/**.cpp", "src/**.inl", "src/**.h", "src/**.c"  }

    filter "configurations:Debug"
            defines { "DEBUG" }

    filter "configurations:Release"
            defines { "NDEBUG" }