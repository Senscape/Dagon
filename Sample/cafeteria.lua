room 'Nursery'

cafeteria1 = Node("cafeteria1", "Door to Hallway B")
cafeteria2 = Node("cafeteria2", "Middle section")
cafeteria3 = Node("cafeteria3", "Door to Hallway C")

cafeteria1:link({ N = cafeteria2, S = Nursery })
cafeteria2:link({ N = cafeteria3, S = cafeteria1 })
cafeteria3:link({ S = cafeteria2 })
