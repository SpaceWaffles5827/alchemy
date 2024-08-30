workspace "AlchemyWorkSpace"
   configurations { "Debug", "Release" }
   platforms { "x64", "ARM64" }

project "AlchemyProject"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "src/**.h", "src/**.cpp", "include/**.h" }
   includedirs { "include", "src", "include/GLFW", "include/GL", "include/freetype2", "include/OpenAL" }
   libdirs { "lib" }

   cppdialect "C++20"

   filter "system:windows"
      defines { "GLFW_INCLUDE_NONE", "GLEW_STATIC", "AL_LIBTYPE_STATIC" }
      links { "glfw3", "glew32s", "freetype", "opengl32", "OpenAL32" }  -- Ensure this is pointing to your static OpenAL32.lib

   filter "system:linux"
      defines { "GLFW_INCLUDE_NONE" }
      links { "GL", "GLU", "glfw", "GLEW", "freetype", "pthread", "openal" }
      buildoptions { "-std=c++20" }

   filter "system:macosx"
      architecture "ARM64"
      defines { "GLFW_INCLUDE_NONE" }
      links { "Cocoa.framework", "IOKit.framework", "CoreVideo.framework", "OpenGL.framework", "glfw", "GLEW", "freetype", "OpenAL.framework" }
      buildoptions { "-std=c++20" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
