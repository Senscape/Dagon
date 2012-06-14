room 'CellsA'
room 'CorridorB'

hasKeyMale = false

hospital1 = Node("hospital1", "Door to Hallway B")
hospital2 = Node("hospital2", "Middle section")
hospital3 = Node("hospital3", "Door to Cells")

function enterCells()
	if hasKeyMale == false then
		feed("I need a key.")
		journalAddEntry(jEntries.KEY_MALE, function() end)
		journalTagEntry(jEntries.KEY_MALE)		
		hasKeyMale = true
	else
		journalMarkEntry(jEntries.KEY_MALE)	
		switch(CellsA)
	end 
end

hospital1:link({ W = hospital2, E = CorridorB })
hospital2:link({ W = hospital3, E = hospital1 })
hospital3:link({ E = hospital2, W = enterCells })
