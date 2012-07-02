room 'CellsA'
room 'CorridorB'

require 'notes'

hasKeyMale = false

hospital_1 = Node("Nodes/hospital_1", "Door to Hallway B")
hospital_2 = Node("Nodes/hospital_2", "Middle section")
hospital_3 = Node("Nodes/hospital_3", "Door to Cells")

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

hospital_1:link({ W = hospital_2, E = CorridorB })
hospital_2:link({ W = hospital_3, E = hospital_1 })
hospital_3:link({ E = hospital_2, W = enterCells })

tube = hospital_1:addSpot( Spot(WEST, {788, 368}) )
tube:attach(VIDEO, "vid_tubea_hospital1.ogv", {loop = true})
	
tube = hospital_1:addSpot( Spot(WEST, {894, 698}) )
tube:attach(VIDEO, "vid_tubeb_hospital1.ogv", {loop = true})
	
tube = hospital_2:addSpot( Spot(EAST, {1118, 306}) )
tube:attach(VIDEO, "vid_tube_hospital2.ogv", {loop = true})
	
tube = hospital_3:addSpot( Spot(EAST, {1030, 534}) )
tube:attach(VIDEO, "vid_tubea_hospital3.ogv", {loop = true})

tube = hospital_3:addSpot( Spot(EAST, {1050, 802}) )
tube:attach(VIDEO, "vid_tubeb_hospital3.ogv", {loop = true})
