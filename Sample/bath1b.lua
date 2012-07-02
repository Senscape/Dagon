room 'CellsA'

bath1b_1 = Node("Nodes/bath1b_1", "Bath")
bath1b_2 = Node("Nodes/bath1b_2", "Bath")

bath1b_1:link({ W = bath1b_2, S = CellsA })
bath1b_2:link({ E = bath1b_1 })
