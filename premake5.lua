Is64bit = false

workspace "WotScript"
    language "C++"

    location "build"

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

    targetdir ("build/bin/%{prj.name}/%{cfg.longname}")

    objdir ("build/obj/%{prj.name}/%{cfg.longname}")

project "Scanner-Preprocessor"

    kind "ConsoleApp"

    files { "src/**.hpp", "src/**.cpp", "src/**.inl", "src/**.h", "src/**.c"  }

    filter "configurations:Debug"
            defines { "DEBUG" }

    filter "configurations:Release"
            defines { "NDEBUG" }