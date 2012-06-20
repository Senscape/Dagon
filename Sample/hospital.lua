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
