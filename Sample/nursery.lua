room 'CorridorB'

nursery1 = Node("nursery1", "Door to Hallway B")
nursery2 = Node("nursery2", "Middle section")
nursery3 = Node("nursery3", "Door to Cells")

nursery1:link({ W = nursery2, E = CorridorB })
nursery2:link({ W = nursery3, E = nursery1 })
nursery3:link({ E = nursery2 })
