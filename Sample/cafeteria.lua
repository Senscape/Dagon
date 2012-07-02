room 'CorridorB'

cafeteria_1 = Node("Nodes/cafeteria_1", "Door to Hallway B")
cafeteria_2 = Node("Nodes/cafeteria_2", "Middle section")
cafeteria_3 = Node("Nodes/cafeteria_3", "Door to Hallway C")

function toCorridorB()
	lookAt(SOUTH)
	switch(CorridorB)
end

cafeteria_1:link({ N = cafeteria_2 })
cafeteria_2:link({ N = cafeteria_3, S = cafeteria_1 })
cafeteria_3:link({ S = cafeteria_2 })

spot = cafeteria_1:addSpot( Spot(SOUTH, {64, 424}) )
spot:attach(AUDIO, "sfx_door_large_opend.ogg" )
spot:attach(VIDEO, "vid_door_cafeteria_corridorb.ogv", {autoplay = false, sync = true} )
spot:attach(FUNCTION, 
	function()
		lookAt(SOUTH)
		self:play()
		switch(CorridorB)
		play("sfx_door_large_closed.ogg")	
	end)
spot:stop()

spot = cafeteria_3:addSpot( Spot(WEST, {1536, 1164, 1625, 1164, 1625, 1290, 1536, 1290}) )
alarm = spot:attach(AUDIO, Audio("sfx_alarm.ogg", {loopable = true}) )
spot:attach(FUNCTION, 
	function()
		if self:isPlaying() == false then 
			self:play()
		else
			self:stop()
		end
	end)

spot = cafeteria_2:addSpot( Spot(NORTH, {1024, 1024}) )
spot:attach(AUDIO, alarm, {volume = 50})

spot = cafeteria_1:addSpot( Spot(NORTH, {1024, 1024}) )
spot:attach(AUDIO, alarm, {volume = 25})
