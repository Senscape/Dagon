room 'CellsA'
room 'CorridorB'

hospital1 = Node("hospital1", "Door to Hallway B")
hospital2 = Node("hospital2", "Middle section")
hospital3 = Node("hospital3", "Door to Cells")

hospital1:link({ W = hospital2, E = CorridorB })
hospital2:link({ W = hospital3, E = hospital1 })
hospital3:link({ W = CellsA, E = hospital2 })
