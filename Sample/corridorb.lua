room 'Cafeteria'
room 'Hospital'

require 'notes'

corridorb_2 = Node("Nodes/corridorb_2", "Door to Cafeteria") -- Becomes the first node
corridorb_1 = Node("Nodes/corridorb_1", "Door to Nursery")
corridorb_3 = Node("Nodes/corridorb_3", "Map")

corridorb_1:link({ W = Hospital, E = corridorb_2 })
corridorb_2:link({ W = corridorb_1, N = Cafeteria, E = corridorb_3 })
corridorb_3:link({ W = corridorb_2 })

spot = corridorb_3:addSpot( Spot(NORTH, {512, 768, 1792, 768, 1792, 1536, 512, 1536}) )
spot:attach(FUNCTION, 
	function()
		feed("Ah, there's the hospital.")
		journal:markEntry(Notes.HOSPITAL)
end)
spot:setCursor(LOOK)

spot = corridorb_2:addSpot( Spot(NORTH, {64, 424}) )
spot:attach(AUDIO, "sfx_door_large_opend.ogg" )
spot:attach(VIDEO, "vid_door_corridorb_cafeteria.ogv", {autoplay = false, sync = true} )
spot:attach(FUNCTION, 
	function()
		lookAt(NORTH)
		self:play()
		switch(Cafeteria)
		play("sfx_door_large_closed.ogg")
	end)
spot:stop()
