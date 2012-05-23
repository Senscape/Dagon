room 'CorridorB'

cafeteria1 = Node("cafeteria1", "Door to Hallway B")
cafeteria2 = Node("cafeteria2", "Middle section")
cafeteria3 = Node("cafeteria3", "Door to Hallway C")

cafeteria1:link({ N = cafeteria2, S = CorridorB })
cafeteria2:link({ N = cafeteria3, S = cafeteria1 })
cafeteria3:link({ S = cafeteria2 })

spot = cafeteria3:addSpot( Spot(WEST, {1536, 1164, 1625, 1164, 1625, 1290, 1536, 1290}) )
alarm = spot:attach(AUDIO, Audio("sfx_alarm.ogg", {loopable = true}) )
spot:attach(CUSTOM, 
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
