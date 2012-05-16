room 'Bath1B'
room 'Nursery'

cellsa1 = Node("cellsa1", "Cells")
cellsa2 = Node("cellsa2", "Cells")
cellsa3 = Node("cellsa3", "Cells")
cellsa4 = Node("cellsa4", "Cells")
cellsa5 = Node("cellsa5", "Cells")
cellsa6 = Node("cellsa6", "Cells")

cellsa1:link({ W = cellsa2, E = Nursery })
cellsa2:link({ W = cellsa3, E = cellsa1 })
cellsa3:link({ W = cellsa4, E = cellsa2 })
cellsa4:link({ N = Bath1B, W = cellsa5, E = cellsa3 })
cellsa5:link({ W = cellsa6, E = cellsa4 })
cellsa6:link({ E = cellsa5 })
