room 'CellsA'

bath1b_1 = Node("bath1_b", "Bath")
bath1b_2 = Node("bath1_b2", "Bath")

bath1b_1:link({ W = bath1b_2, S = CellsA })
bath1b_2:link({ E = bath1b_1 })
