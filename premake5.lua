workspace "AlchemyWorkSpace"
   configurations { "Debug", "Release" }
   platforms { "x64", "ARM64" }

project "AlchemyProject"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "src/**.h", "src/**.cpp", "include/**.h" }
   includedirs { "include", "src", "include/GLFW", "include/GL", "include/freetype2" }
   libdirs { "lib" }

   cppdialect "C++20"  -- Use C++20 standard

   filter "system:windows"
      defines { "GLFW_INCLUDE_NONE", "GLEW_STATIC" }
      links { "glfw3", "glew32s", "freetype", "opengl32" }

   filter "system:linux"
      defines { "GLFW_INCLUDE_NONE" }
      links { "GL", "GLU", "glfw", "GLEW", "freetype", "pthread" }
      buildoptions { "-std=c++20" }

   filter "system:macosx"
      architecture "ARM64"  -- Ensure it's set for ARM64 on M1/M2 Macs
      defines { "GLFW_INCLUDE_NONE" }
      links { "Cocoa.framework", "IOKit.framework", "CoreVideo.framework", "OpenGL.framework", "glfw", "GLEW", "freetype" }
      buildoptions { "-std=c++20" }  -- Ensure C++20 is used

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

