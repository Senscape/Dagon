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

require "mod_tracks"

-- We put all the transitions with door animations here. Notice how we make use
-- of a generic function that moves the camera starts the video. With this
-- scheme in mind, we only need to provide direction and destination node.
function _generic_transition(direction, node, sfx_open, sfx_close)
	door_open = Audio.new(sfx_open or "sfx_door_creaky_opend.ogg")
	door_close = Audio.new(sfx_close or "sfx_door_creaky_closed.ogg")
	
	if (config.scare == false) then
		camera.lookat(direction, 0, false)
	end
	door_open:play()
	
	-- "self" will always reference to the last clicked spot - very useful!
	self:play()
	
	-- Video sync is not yet available so we manually wait a few seconds
	sleep(2250)
	
	switch(node)
	door_close:play()
end

-- The following functions handle the audio FX and in particular the footsteps,
-- which are not supported by the engine but are very easy to reproduce with its
-- features. Basically, we randomize the effects each time a node is entered.
MAX_STEPS = 6

-- Function to globally handle every time a node is entered
function _on_enter_node()
	current = Node.current()
		
	-- Randomize channel from the available footsteps
	idx = math.random(1, MAX_STEPS)

	if current == corridorb1 or current == corridorb2 or
		current == cellsa1 or current == cellsa2 or 
		current == cellsa3 or current == cellsa4 or 
		current == cellsa5 or current == cellsa6 then
		step_type = "concrete"
		suffix = "_h"
	elseif current == cafeteria1 or current == cafeteria2 or
		current == cafeteria3 then
		step_type = "concrete"
		suffix = "_l"
	elseif current == hospital1 or current == hospital2 or
		current == hospital3 then
		step_type = "marble"
		suffix = "_m"
	else
		step_type = "marble"
		suffix = "_m"
	end
	
	step = Audio.new("sfx_step_" .. step_type .. suffix .. idx .. ".ogg")
	
	step:play()
end

function to_heavy_breathe()
	config.scare = true
	breathe:fadeout(500)
	heavy:volume(1)
	heavy:play()
end

function to_normal_breathe()
	config.scare = false
	heavy:fadeout(250)
	breathe:volume(0.1)
end

-- This is our custom function that handles all the audio initialization
function init_audio()
	breathe = Audio.new("sfx_breathe.ogg", true)
	breathe:volume(0.1)
	breathe:play()
	
	heavy = Audio.new("sfx_heavy_breathe.ogg")
	
	tone = Audio.new("sfx_room_tone.ogg", true)
	tone:volume(0.1)
	
	jail = Audio.new("sfx_jail.ogg", true)
	jail:volume(0.5)
	
	cells = Audio.new("sfx_cells.ogg", true)
	cells:volume(0.25)
	
	hospital = Audio.new("sfx_hospital.ogg", true)
	hospital:volume(0.5)
	
	-- This utilizes the Tracks module, which enables dynamic music
	daniel = Track.new("trck_daniel")	
	daniel:active({0})
	daniel:play()
end

function create_first_floor()
	-- Generic room to hold the nodes (temporary)
	generic = Room.new("generic")
	
	-- Cafeteria is declared first because it's our entry point
	room "cafeteria"
	room "cellsa"
	room "corridorb"
	room "hospital"
end
