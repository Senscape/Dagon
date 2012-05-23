room 'Cafeteria'
room 'Hospital'

corridorb2 = Node("corridorb2", "Door to Cafeteria")
corridorb1 = Node("corridorb1", "Door to Nursery")
corridorb3 = Node("corridorb3", "Map")

corridorb1:link({ W = Hospital, E = corridorb2 })
corridorb2:link({ W = corridorb1, N = Cafeteria, E = corridorb3 })
corridorb3:link({ W = corridorb2 })

spot = corridorb3:addSpot( Spot(NORTH, {512, 768, 1792, 768, 1792, 1536, 512, 1536}) )
spot:attach(CUSTOM, 
	function()
		feed("Ah, there's the hospital.")
		journalMarkEntry(jEntries.HOSPITAL)
end)
