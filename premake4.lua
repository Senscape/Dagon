-- Dagon - An Adventure Game Engine
solution "Dagon"
  configurations { "Release", "Debug" }
  platforms { "native", "universal" }
  location "Build"
  
  configuration { "Release" }
  defines { "NDEBUG" }
  flags { "Optimize" }
  targetdir "Build/Release"  
      
  configuration { "Debug" }
  defines { "_DEBUG", "DEBUG" }
  flags { "Symbols" }
  targetdir "Build/Debug"

  if _ACTION == "clean" then
    os.rmdir("Build")
  end
 
  project "Dagon"
    targetname "dagon"
    defines { "GLEW_STATIC", "OV_EXCLUDE_STATIC_CALLBACKS" }
    location "Build"
    objdir "Build/Objects"
     
    kind "ConsoleApp"
    language "C++"
    files { "Source/**.h", "Source/**.c", "Source/**.cpp" }
    
    libraries = { "freetype", "GLEW", "lua", "ogg", "SDL2",
                  "vorbis", "vorbisfile", "theoradec" }
                  
    if os.get() == "linux" then
      for i = 1, #libraries do
        local lib = libraries[i]
        if os.findlib(lib) == nil then
          print ("Library not found:", lib)
          print "Aborting..."
          os.exit()
        end
      end
    end
      
    links { libraries }
  
    configuration "macosx"
      includedirs { "Mac/Headers", "Mac/Headers/freetype2" }
      libdirs { "Mac/Libraries" }
    
      links { "AudioToolbox.framework", "AudioUnit.framework", 
              "Carbon.framework", "Cocoa.framework", 
              "CoreAudio.framework", "CoreFoundation.framework",
              "ForceFeedback.framework", "IOKit.framework",
              "OpenAL.framework", "OpenGL.framework" }
              
    configuration "windows"
      includedirs { "Windows/Headers", "Windows/Headers/freetype" }
      libdirs { "Windows/Libraries" }
      
      links { "opengl32", "glu32", "Imm32", "version", "winmm" }
 