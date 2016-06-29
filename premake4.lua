--[[

DAGON
An Adventure Game Engine

This is a fairly basic Premake configuration that generates project files for
your preferred build system. Premake may be downloaded from the following
site:

http://industriousone.com/premake

Usage is as simple as typing 'premake4 [action]'. Because this Premake file is
primarily intended for building from source and contributing to Dagon, we
strongly suggest that you download the official binary releases for deploying
your games:

https://github.com/Senscape/Dagon/releases

Copyright (c) 2011-2014 Senscape s.r.l. All rights reserved.

This Source Code Form is subject to the terms of the Mozilla Public License,
v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain
one at http://mozilla.org/MPL/2.0/.

--]]

--[[

BUILD INSTRUCTIONS

Dagon requires the following dependencies: FreeType, GLEW, Lua 5.1, Ogg, OpenAL,
Theora, Vorbis, SDL2.

Linux:

  We suggest installing the following packages via apt-get: libfreetype6-dev,
  libglew-dev, liblua5.1-0-dev, libogg-dev, libopenal-dev, libvorbis-dev,
  libtheora-dev, libsdl2-dev.

  SDL2 was not available in some default repos as of this writing and had to be
  built from scratch.

Mac OS X:

  Libraries are included in the extlibs folder. You may also use Homebrew to
  install custom packages: http://brew.sh

  Suggested Homebrew formulas are: freetype, glew, lua, libogg, libvorbis,
  theora, sdl2.

Windows:

  Libraries for Visual Studio are included in the extlibs folder.

]]--

-- Base solution
solution "Dagon"
  configurations { "debug", "release" }
  platforms { "native" }
  location "build"
      
  configuration { "debug" }
  defines { "_DEBUG", "DEBUG" }
  flags { "Symbols" }
  targetdir "build/debug"

  configuration { "release" }
  defines { "NDEBUG" }
  flags { "Optimize" }
  targetdir "build/release"    

  -- Clean up if required and exit
  if _ACTION == "clean" then
    os.rmdir("build")
  end
 
  -- The main Dagon project
  project "Dagon"
    targetname "dagon"
    -- GLEW_STATIC only applies to Windows, but there's no harm done if defined
    -- on other systems.
    defines { "GLEW_STATIC", "OV_EXCLUDE_STATIC_CALLBACKS", "KTX_OPENGL" }
    location "build"
    objdir "build/objs"
    buildoptions { "-Wall" }
     
    -- Note that we always build as a console app, even on Windows. Please use
    -- the corresponding Xcode or Visual Studio project files to build a
    -- user-friendly binary.
    kind "ConsoleApp"
    language "C++"
    files { "src/**.h", "src/**.c", "src/**.cpp" }
    
    -- Libraries required for Unix-based systems
    libs_unix = { "freetype", "GLEW", "GL", "GLU", "ogg", "openal", "vorbis",
		  "vorbisfile", "theoradec", "SDL2", "m", "stdc++" }
  
    -- Search for libraries on Linux systems
    if os.get() == "linux" then
      -- Attempt to look for Lua library with most commonly used names
      local lua_lib_names = { "lua-5.1", "lua5.1", "lua" }
      local lua_lib = { name = nil, dir = nil }
      for i = 1, #lua_lib_names do
        lua_lib.name = lua_lib_names[i]
        lua_lib.dir = os.findlib(lua_lib.name)
        if(lua_lib.dir ~= nil) then
          break
        end
      end
      table.insert(libs_unix, lua_lib.name)

      -- Confirm that all the required libraries are present
      for i = 1, #libs_unix do
        local lib = libs_unix[i]
        if os.findlib(lib) == nil then
          print ("WARNING: Library " .. lib .. " not found")
        end
      end
    end

    -- Final configuration, includes and links according to the host system
    configuration "linux"
      includedirs { "/usr/include", "/usr/include/lua5.1",
                    "/usr/include/freetype2", "/usr/local/include",
                    "/usr/local/include/lua5.1",
                    "/usr/local/include/freetype2" }
      libdirs { "/usr/lib", "/usr/local/lib" }
      links { libs_unix, "dl" }
      linkoptions { "-pthread" }

    configuration "bsd"
      includedirs { "/usr/include",
                    "/usr/local/include/freetype2",
                    "/usr/local/include" }
      libdirs { "/usr/lib", "/usr/local/lib" }
      buildoptions { "`pkg-config --cflags lua-5.1`" }
      linkoptions { "`pkg-config --libs lua-5.1`" }
      links { libs_unix }
      linkoptions { "-pthread" }
  
    configuration "macosx"
      includedirs { "/usr/include", "/usr/include/lua5.1", 
                    "/usr/include/freetype2", "/usr/local/include",
                    "/usr/local/include/lua5.1",
                    "/usr/local/include/freetype2",
                    "extlibs/headers",
                    "extlibs/headers/libfreetype/osx",
                    "extlibs/headers/libfreetype/osx/freetype2",
                    "extlibs/headers/libsdl2/osx" }
      libdirs { "/usr/lib", "/usr/local/lib", 
                "extlibs/libs-osx/Frameworks", "extlibs/libs-osx/lib" }
      links { "freetype", "GLEW", "lua", "ogg", "SDL2", 
              "vorbis", "vorbisfile", "theoradec", "ktx" }
      links { "AudioToolbox.framework", "AudioUnit.framework",
              "Carbon.framework", "Cocoa.framework", "CoreAudio.framework",
              "CoreFoundation.framework", "ForceFeedback.framework", 
              "IOKit.framework", "OpenAL.framework", "OpenGL.framework" }
              
    configuration "windows"
      includedirs { "extlibs/headers",
                    "extlibs/headers/libfreetype/windows",
                    "extlibs/headers/libfreetype/windows/freetype",
                    "extlibs/headers/libsdl2/windows",
                    "extlibs/headers/dirent" }
      links { "freetype", "glew32s", "libogg_static", 
              "libtheora_static", "libvorbis_static", 
              "libvorbisfile_static", "lua", "OpenAL32",
              "SDL2", "SDL2main", "opengl32", "glu32",
              "Imm32", "version", "winmm", "libktx" }
      if os.is64bit then
        libdirs { "extlibs/libs-msvc/x64" }
      else
        libdirs { "extlibs/libs-msvc/x86" }
      end
