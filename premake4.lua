--[[

DAGON
An Adventure Game Engine

This is a fairly basic Premake configuration that generates project files for
your preferred build system. Premake may be downloaded from the following
site:

http://industriousone.com/premake

Usage is as simple as typing 'premake4 [action]'. Please, do note that this
configuration targets the native architecture of the host computer and will
attempt to link dynamically. Also, the Windows configuration only builds as
a console.

Because this Premake file is primarily intended for building from source and
contributing to Dagon, we strongly suggest that you download the official binary
releases for deploying your games:

https://github.com/Senscape/Dagon/releases

Copyright (c) 2011-2013 Senscape s.r.l. All rights reserved.

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
  libtheora-dev.

  SDL2 was not available in default repos as of this writing and had to be built
  from scratch.

Mac OS X:

  We strongly encourage using Homebrew to install packages: http://brew.sh

  Suggested Homebrew formulas are: freetype, glew, lua, libogg, libvorbis,
  theora, sdl2.

Windows:

  We provide the following Visual Studio binaries for your convenience:

  http://www.senscape.net/files/dagon-0.6.0-libs-win-x86.zip
  http://www.senscape.net/files/dagon-0.6.0-libs-win-x64.zip

  Please note that on Visual Studio you may need to specify the target platform
  as well as includes and libraries folders.

]]--

-- Base solution
solution "Dagon"
  configurations { "Release", "Debug" }
  platforms { "native", "x32", "x64" }
  location "Build"
  
  configuration { "Release" }
  defines { "NDEBUG" }
  flags { "Optimize" }
  targetdir "Build/Release"  
      
  configuration { "Debug" }
  defines { "_DEBUG", "DEBUG" }
  flags { "Symbols" }
  targetdir "Build/Debug"

  -- Clean up if required and exit
  if _ACTION == "clean" then
    os.rmdir("Build")
    os.exit()
  end
 
  -- The main Dagon project
  project "Dagon"
    targetname "dagon"
    -- GLEW_STATIC only applies to Windows, but there's no harm done if defined
    -- on other systems.
    defines { "GLEW_STATIC", "OV_EXCLUDE_STATIC_CALLBACKS" }
    location "Build"
    objdir "Build/Objects"
     
    -- Note that we always build as a console app, even on Windows.
    kind "ConsoleApp"
    language "C++"
    files { "Source/**.h", "Source/**.c", "Source/**.cpp" }
    
    libraries = {}
    
    -- Libraries required for Unix-based systems
    libs_unix = { "freetype", "GLEW", "ogg", "SDL2", 
                  "vorbis", "vorbisfile", "theoradec" }
    
    -- Libraries required for Windows, preferring static binaries
    libs_win = { "freetype", "glew32s", "libogg_static", 
                 "libtheora_static", "libvorbis_static", 
                 "libvorbisfile_static", "lua", "OpenAL32",
                 "SDL2", "SDL2main", "opengl32", "glu32",
                 "Imm32", "version", "winmm" }

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
  
    -- Build the libraries table according to the host system
    if os.is("linux") then
      libraries = libs_unix
      table.insert(libraries, "GL")
      table.insert(libraries, "GLU")
      table.insert(libraries, "openal")
      table.insert(libraries, lua_lib.name)
    elseif os.is("macosx") then
      libraries = libs_unix
      table.insert(libraries, lua_lib.name)
    elseif os.is("windows") then
      libraries = libs_win
      table.insert(libraries, lua_lib.name)
    else
      print "Attempting to build on unsupported system."
      print "Aborting..."
      os.exit()
    end
    
    -- Confirm that all the required libraries are present
    -- (not working well on Windows)
    if os.is("linux") or os.is("macosx") then
      for i = 1, #libraries do
        local lib = libraries[i]
        if os.findlib(lib) == nil then
          print ("Library not found:", lib)
          print "Aborting..."
          os.exit()
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
      links { libraries }
  
    configuration "macosx"
      includedirs { "/usr/include", "/usr/include/lua5.1", 
                    "/usr/include/freetype2", "/usr/local/include",
                    "/usr/local/include/lua5.1",
                    "/usr/local/include/freetype2" }
      libdirs { "/usr/lib", "/usr/local/lib" }
      links { "OpenAL.framework", "OpenGL.framework" }
      links { libraries }
              
    configuration "windows"
      links { libraries }
