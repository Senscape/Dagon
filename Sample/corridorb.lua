room 'Cafeteria'
room 'Hospital'

require 'notes'

corridorb2 = Node("corridorb2", "Door to Cafeteria")
corridorb1 = Node("corridorb1", "Door to Nursery")
corridorb3 = Node("corridorb3", "Map")

corridorb1:link({ W = Hospital, E = corridorb2 })
corridorb2:link({ W = corridorb1, N = Cafeteria, E = corridorb3 })
corridorb3:link({ W = corridorb2 })

spot = corridorb3:addSpot( Spot(NORTH, {512, 768, 1792, 768, 1792, 1536, 512, 1536}) )
spot:attach(FUNCTION, 
	function()
		feed("Ah, there's the hospital.")
		journal:markEntry(Notes.HOSPITAL)
end)
spot:setCursor(LOOK)

spot = corridorb2:addSpot( Spot(NORTH, {64, 424}) )
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