--[[

ASYLUM

Script file: CellsA room

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

function create_cellsa()
	cellsa1 = Node.new("cellsa1")
	cellsa2 = Node.new("cellsa2")
	cellsa3 = Node.new("cellsa3")
	cellsa4 = Node.new("cellsa4")
	cellsa5 = Node.new("cellsa5")
	cellsa6 = Node.new("cellsa6")
end

---------------------------------------------------------------------------------
-- Node linking
---------------------------------------------------------------------------------

function link_cellsa()
	cellsa1:link({ W = cellsa2 })
								 
	cellsa2:link({ W = cellsa3, E = cellsa1 })
	
	cellsa3:link({ W = cellsa4, E = cellsa2 })

	cellsa4:link({ W = cellsa5, E = cellsa3 })
	
	cellsa5:link({ W = cellsa6, E = cellsa4 })
	
	cellsa6:link({ E = cellsa5 })
	
	-- Door 1
	door = Spot.new(EAST, {64, 424})
	door:attach(CUSTOM, from_cellsa_to_hospital)
	door:attach(VIDEO, "vid_door_cellsa_hospital.ogv")
	cellsa1:addspot(door)
	
	-- Madman 1
	mad = Spot.new(SOUTH, {947, 1047})
	mad:attach(AUDIO, "sfx_mad_cellsa2.ogg", {loop = true})
	mad:attach(VIDEO, "vid_mad_cellsa2.ogv", {loop = true})
	mad:play()
	cellsa2:addspot(mad)
	
	-- Madman 2
	mad = Spot.new(NORTH, {1036, 1079})
	mad:attach(AUDIO, "sfx_mad_cellsa5.ogg", {loop = true})
	mad:attach(VIDEO, "vid_mad_cellsa5.ogv", {loop = true})
	mad:play()
	cellsa5:addspot(mad)
	
	-- Madman 3
	mad = Spot.new(NORTH, {406, 824})
	mad:attach(CUSTOM, trigger_scare)
	mad:attach(AUDIO, "sfx_mad_scare.ogg")
	mad:attach(VIDEO, "vid_mad_scare.ogv")
	cellsa6:addspot(mad)
end

---------------------------------------------------------------------------------
-- Transitions
---------------------------------------------------------------------------------

function from_cellsa_to_hospital()
	_generic_transition(EAST, hospital3, 
		"sfx_door_metal_opend.ogg", "sfx_door_metal_closed.ogg")
	config.throb = true
	cells:volume(0.5)
	hospital:play()
	daniel:active({ 0, 1 })
end

---------------------------------------------------------------------------------
-- Interaction
---------------------------------------------------------------------------------

function trigger_scare()
	self:play()
	timer.start(3, to_heavy_breathe)
	timer.start(6, to_normal_breathe)
	timer.start(9, function() self:disable() end)
end
