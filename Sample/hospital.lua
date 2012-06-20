room 'CellsA'
room 'CorridorB'

require 'notes'

hasKeyMale = false

hospital1 = Node("hospital1", "Door to Hallway B")
hospital2 = Node("hospital2", "Middle section")
hospital3 = Node("hospital3", "Door to Cells")

function enterCells()
	if hasKeyMale == false then
		feed("I need a key.")
		journal:addEntry(Notes.KEY_MALE, function() end)
		journal:tagEntry(Notes.KEY_MALE)		
		hasKeyMale = true
	else
		journal:markEntry(Notes.KEY_MALE)	
		switch(CellsA)
	end 
end

hospital1:link({ W = hospital2, E = CorridorB })
hospital2:link({ W = hospital3, E = hospital1 })
hospital3:link({ E = hospital2, W = enterCells })

tube = hospital1:addSpot( Spot(WEST, {788, 368}) )
tube:attach(VIDEO, "vid_tubea_hospital1.ogv", {loop = true})
	
tube = hospital1:addSpot( Spot(WEST, {894, 698}) )
tube:attach(VIDEO, "vid_tubeb_hospital1.ogv", {loop = true})
	
tube = hospital2:addSpot( Spot(EAST, {1118, 306}) )
tube:attach(VIDEO, "vid_tube_hospital2.ogv", {loop = true})
	
tube = hospital3:addSpot( Spot(EAST, {1030, 534}) )
tube:attach(VIDEO, "vid_tubea_hospital3.ogv", {loop = true})

tube = hospital3:addSpot( Spot(EAST, {1050, 802}) )
tube:attach(VIDEO, "vid_tubeb_hospital3.ogv", {loop = true})
