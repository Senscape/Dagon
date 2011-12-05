--[[

ASYLUM

Script file: Hospital room

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

function create_hospital()
	hospital1 = Node.new("hospital1")
	hospital2 = Node.new("hospital2")
	hospital3 = Node.new("hospital3")
end

---------------------------------------------------------------------------------
-- Node linking
---------------------------------------------------------------------------------

function link_hospital()
	hospital1:link({ W = hospital2 })
	
	hospital2:link({ E = hospital1, W = hospital3 })		

	hospital3:link({ E = hospital2 })
	
	-- Door 1
	door = Spot.new(EAST, {64, 424})
	door:attach(CUSTOM, from_hospital_to_corridorb)
	door:attach(VIDEO, "vid_door_hospital_corridorb.ogv")
	hospital1:addspot(door)
	
	-- Door 2
	door = Spot.new(WEST, {64, 424})
	door:attach(CUSTOM, from_hospital_to_cellsa)
	door:attach(VIDEO, "vid_door_hospital_cellsa.ogv")
	hospital3:addspot(door)
	
	-- Blinking tube 1
	tube = Spot.new(WEST, {788, 368})
	tube:attach(VIDEO, "vid_tubea_hospital1.ogv", {loop = true})
	tube:play()
	hospital1:addspot(tube)
	
	-- Blinking tube 2
	tube = Spot.new(WEST, {894, 698})
	tube:attach(VIDEO, "vid_tubeb_hospital1.ogv", {loop = true})
	tube:play()
	hospital1:addspot(tube)	
	
	-- Blinking tube 3
	tube = Spot.new(EAST, {1118, 306})
	tube:attach(VIDEO, "vid_tube_hospital2.ogv", {loop = true})
	tube:play()
	hospital2:addspot(tube)	
	
	-- Blinking tube 3
	tube = Spot.new(EAST, {1030, 534})
	tube:attach(VIDEO, "vid_tubea_hospital3.ogv", {loop = true})
	tube:play()
	hospital3:addspot(tube)
	
	-- Blinking tube 4
	tube = Spot.new(EAST, {1050, 802})
	tube:attach(VIDEO, "vid_tubeb_hospital3.ogv", {loop = true})
	--tube:play()
	hospital3:addspot(tube)		
end

---------------------------------------------------------------------------------
-- Transitions
---------------------------------------------------------------------------------

function from_hospital_to_cellsa()
	_generic_transition(WEST, cellsa1, 
											"sfx_door_metal_opend.ogg", "sfx_door_metal_closed.ogg")
	cells:volume(1)
	daniel:active({ 0, 1, 2 })
end

function from_hospital_to_corridorb()
	_generic_transition(EAST, corridorb1,
												"sfx_door_large_opend.ogg", "sfx_door_large_closed.ogg")
	cells:stop()
	hospital:stop()	
	jail:volume(0.5)
	daniel:active({ 0 })
	config.throb = false
end
