--[[

ASYLUM

Script file: CorridorB room

Copyright (c) 2011 Senscape s.r.l.
All rights reserved.

NOTICE: Senscape permits you to use, modify, and distribute this
file in accordance with the terms of the license agreement accompanying it.

All the assets referenced by this code are the sole property of Senscape s.r.l.
and you are not allowed to re-distribute them in any way.

--]]

---------------------------------------------------------------------------------
-- Node creation
---------------------------------------------------------------------------------

function create_corridorb()
	corridorb1 = Node.new("corridorb1")
	corridorb2 = Node.new("corridorb2")
end

---------------------------------------------------------------------------------
-- Node linking
---------------------------------------------------------------------------------

function link_corridorb()
	corridorb1:link({ E = corridorb2 })
	
	corridorb2:link({ W = corridorb1 })
		
	door = Spot.new(WEST, {64, 424})
	door:attach(CUSTOM, from_corridorb_to_hospital)
	door:attach(VIDEO, "vid_door_corridorb_hospital.ogv")
	corridorb1:addspot(door)

	door = Spot.new(NORTH, {64, 424})
	door:attach(CUSTOM, from_corridorb_to_cafeteria)
	door:attach(VIDEO, "vid_door_corridorb_cafeteria.ogv")
	corridorb2:addspot(door)
end

---------------------------------------------------------------------------------
-- Transitions
---------------------------------------------------------------------------------

function from_corridorb_to_cafeteria()
	_generic_transition(NORTH, cafeteria1,
		"sfx_door_large_opend.ogg", "sfx_door_large_closed.ogg")
	jail:stop() 
	tone:play()
end

function from_corridorb_to_hospital()
	local door_open = Audio.new("sfx_door_large_opend.ogg")
	local door_close = Audio.new("sfx_door_large_closed.ogg")
	
	camera.lookat(WEST, 0, false)
	door_open:play()
	
	self:play()
	config.throb = true
	hospital:play()
	hospital:volume(0.2)
	sleep(2250)
	
	switch(hospital1)
	door_close:play()

	cells:play()
	hospital:volume(0.5)
	jail:volume(1)
	daniel:active({ 0, 1 })
end
