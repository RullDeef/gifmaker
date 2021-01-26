workspace "Giffer"
    configurations { "DEBUG" }

project "GifMaker"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "."

    files {
        "GifMaker/src/**.cpp",
        "GifMaker/src/**.c",
    }

    includedirs {
        "GifMaker/include"
    }
