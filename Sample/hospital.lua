room 'Cafeteria'

hospital1 = Hospital:addNode( Node("hospital1", "Door to Hallway B") )
hospital2 = Hospital:addNode( Node("hospital2", "Middle section") )
hospital3 = Hospital:addNode( Node("hospital3", "Door to Cells") )

hospital1:link({ W = hospital2, E = Cafeteria })
hospital2:link({ W = hospital3, E = hospital1 })
hospital3:link({ E = hospital2 })
