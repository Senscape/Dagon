room 'CorridorB'

cafeteria1 = Node("cafeteria1", "Door to Hallway B")
cafeteria2 = Node("cafeteria2", "Middle section")
cafeteria3 = Node("cafeteria3", "Door to Hallway C")

function toCorridorB()
	lookAt(SOUTH)
	switch(CorridorB)
end

cafeteria1:link({ N = cafeteria2 })
cafeteria2:link({ N = cafeteria3, S = cafeteria1 })
cafeteria3:link({ S = cafeteria2 })

-- Inherit values from spot
-- Stop creates corrupted first frames
-- Maintain convenience tags... Videos only exist in spots or cutscenes, audios can be shared
spot = cafeteria1:addSpot( Spot(SOUTH, {64, 424}) )
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

spot = cafeteria3:addSpot( Spot(WEST, {1536, 1164, 1625, 1164, 1625, 1290, 1536, 1290}) )
alarm = spot:attach(AUDIO, Audio("sfx_alarm.ogg", {loopable = true}) )
spot:attach(FUNCTION, 
	function()
		if self:isPlaying() == false then 
			self:play()
		else
			self:stop()
		end
	end)

spot = cafeteria2:addSpot( Spot(NORTH, {1024, 1024}) )
spot:attach(AUDIO, alarm, {volume = 50})

spot = cafeteria1:addSpot( Spot(NORTH, {1024, 1024}) )
spot:attach(AUDIO, alarm, {volume = 25})
