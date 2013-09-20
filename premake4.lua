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
    
    libs_unix = { "freetype", "GL", "GLU", "GLEW", "lua5.1", "openal",
                  "ogg", "SDL2", "vorbis", "vorbisfile", "theoradec" }
              
    if os.get() == "linux" then
      for i = 1, #libs_unix do
        local lib = libs_unix[i]
        if os.findlib(lib) == nil then
          print ("Library not found:", lib)
          print "Aborting..."
          os.exit()
        end
      end
    end

    configuration "linux"
      includedirs { "/usr/include", "/usr/include/lua5.1", 
                    "/usr/include/freetype2", "/usr/local/include",
                    "/usr/local/include/lua5.1", 
                    "/usr/local/include/freetype2" }
      libdirs { "/usr/lib", "/usr/local/lib" }
      links { libs_unix }
  
    configuration "macosx"
      includedirs { "Mac/Headers", "Mac/Headers/freetype2" }
      libdirs { "Mac/Libraries" }
      links { "AudioToolbox.framework", "AudioUnit.framework", 
              "Carbon.framework", "Cocoa.framework", 
              "CoreAudio.framework", "CoreFoundation.framework",
              "ForceFeedback.framework", "IOKit.framework",
              "OpenAL.framework", "OpenGL.framework" }
      links { "freetype", "GLEW", "lua", "ogg", "SDL2",
              "vorbis", "vorbisfile", "theoradec" }
              
    configuration "windows"
      includedirs { "Windows/Headers", "Windows/Headers/freetype" }
      configuration "x32"
        libdirs { "Windows/Libraries/x86" }
      configuration "x64"
        libdirs { "Windows/Libraries/x64" }
      links { "freetype", "glew32s", "libogg_static", 
              "libtheora_static", "libvorbis_static", 
              "libvorbisfile_static", "lua", "OpenAL32",
              "SDL2", "SDL2main", "opengl32", "glu32",
              "Imm32", "version", "winmm" }
 
