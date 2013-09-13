-- A solution contains projects, and defines the available configurations
solution "Dagon"
   configurations { "Debug", "Release" }
   platforms { "x64" }
   location "Build"
      
   configuration { "Debug" }
   targetdir "Build/Debug"
 
   configuration { "Release" }
   targetdir "Build/Release"

  if _ACTION == "clean" then
    os.rmdir("Build")
  end   
 
   -- A project defines one build target
   project "Dagon"
      targetname "dagon"
      location "Build"
      objdir "Build/Objects"
   
      kind "ConsoleApp"
      language "C++"
      files { "Source/**.h", "Source/**.c", "Source/**.cpp" }

      includedirs { "Libraries/Headers", "Libraries/Headers/freetype2" }
      libdirs { "Libraries/Mac" }

      links { "AudioToolbox.framework", "AudioUnit.framework", 
              "Carbon.framework", "Cocoa.framework", 
              "CoreAudio.framework", "CoreFoundation.framework",
              "ForceFeedback.framework", "IOKit.framework",
              "OpenAL.framework", "OpenGL.framework" }
      links { "freetype", "GLEW", "lua", "ogg", "SDL2",
              "vorbis", "vorbisfile", "theoradec" }
 
      configuration "Debug"
         defines { "_DEBUG", "DEBUG" }
         flags { "Symbols" }
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }
