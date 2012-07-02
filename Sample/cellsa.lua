room 'Bath1B'
room 'Hospital'

cellsa_1 = Node("Nodes/cellsa_1", "Cells")
cellsa_2 = Node("Nodes/cellsa_2", "Cells")
cellsa_3 = Node("Nodes/cellsa_3", "Cells")
cellsa_4 = Node("Nodes/cellsa_4", "Cells")
cellsa_5 = Node("Nodes/cellsa_5", "Cells")
cellsa_6 = Node("Nodes/cellsa_6", "Cells")

cellsa_1:link({ W = cellsa_2, E = Hospital })
cellsa_2:link({ W = cellsa_3, E = cellsa_1 })
cellsa_3:link({ W = cellsa_4, E = cellsa_2 })
cellsa_4:link({ N = Bath1B, W = cellsa_5, E = cellsa_3 })
cellsa_5:link({ W = cellsa_6, E = cellsa_4 })
cellsa_6:link({ E = cellsa_5 })

mad = cellsa_2:addSpot( Spot(SOUTH, {947, 1047}) )
mad:attach(VIDEO, "vid_mad_cellsa2.ogv", {loop = true})

mad = cellsa_5:addSpot( Spot(NORTH, {1036, 1079}) )
mad:attach(VIDEO, "vid_mad_cellsa5.ogv", {loop = true})

mad = cellsa_6:addSpot( Spot(NORTH, {406, 824}) )
mad:attach(VIDEO, "vid_mad_scare.ogv", {autoplay = false, sync = false })
mad:attach(FUNCTION, 
	function()
		self:play()
	end)
mad:stop()

-- The script must perform a check of the spot, or perhaps
-- export a function from DGControl that does all that. Yeah, better....