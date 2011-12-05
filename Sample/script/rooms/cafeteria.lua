--[[

ASYLUM

Script file: Cafeteria room

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

function create_cafeteria()
	cafeteria2 = Node.new("cafeteria2")
	cafeteria1 = Node.new("cafeteria1")
	cafeteria3 = Node.new("cafeteria3")

end

---------------------------------------------------------------------------------
-- Node linking
---------------------------------------------------------------------------------

function link_cafeteria()
	cafeteria1:link({ N = cafeteria2 })
	
	cafeteria2:link({ N = cafeteria3, S = cafeteria1 })
	
	cafeteria3:link({ S = cafeteria2 })

	-- Door to CorridorB	
	door = Spot.new(SOUTH, {64, 424})
	door:attach(CUSTOM, from_cafeteria_to_corridorb)
	door:attach(VIDEO, "vid_door_cafeteria_corridorb.ogv")
	cafeteria1:addspot(door)
end

---------------------------------------------------------------------------------
-- Transitions
---------------------------------------------------------------------------------

function from_cafeteria_to_corridorb()
	_generic_transition(SOUTH, corridorb2,
		"sfx_door_large_opend.ogg", "sfx_door_large_closed.ogg")
		
	jail:play()
	tone:stop()	
end
