room 'Cafeteria'
room 'Hospital'

corridorb2 = Node("corridorb2", "Door to Cafeteria")
corridorb1 = Node("corridorb1", "Door to Nursery")
corridorb3 = Node("corridorb3", "Map")

corridorb1:link({ W = Hospital, E = corridorb2 })
corridorb2:link({ W = corridorb1, N = Cafeteria, E = corridorb3 })
corridorb3:link({ W = corridorb2 })
