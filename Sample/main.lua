--[[

ASYLUM

Script file: Main module

Copyright (c) 2011 Senscape s.r.l.
All rights reserved.

NOTICE: Senscape permits you to use, modify, and distribute this
file in accordance with the terms of the license agreement accompanying it.

All the assets referenced by this code are the sole property of Senscape s.r.l.
and you are not allowed to re-distribute them in any way.

--]]

-- This file is automatically called by engine. Usually, we override settings and
-- prepare things, as well as naming the script.

require "asylum"

config.script = "Asylum"

-- Uncomment this line to use a different extension for textures such as .tga
--config.tex_extension = "tga"

-- Set to false if you want to be an awful person and remove the smashingly cool
-- Dagon logo every single time you launch the script
config.show_splash = false

-- The system must be initialized after any overrides but before everything else
system.init()

create_first_floor()

-- Initialize audio related stuff
init_audio()

-- Switch to our generic room (please note this is temporary as we are using a
-- single room for all the nodes... bad practice!)
switch(generic)

-- Register our global handler that is called whenever a node is entered
register(GLOBAL_ENTER_NODE, _on_enter_node)
