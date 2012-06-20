room 'Bath1B'
room 'Hospital'

cellsa1 = Node("cellsa1", "Cells")
cellsa2 = Node("cellsa2", "Cells")
cellsa3 = Node("cellsa3", "Cells")
cellsa4 = Node("cellsa4", "Cells")
cellsa5 = Node("cellsa5", "Cells")
cellsa6 = Node("cellsa6", "Cells")

cellsa1:link({ W = cellsa2, E = Hospital })
cellsa2:link({ W = cellsa3, E = cellsa1 })
cellsa3:link({ W = cellsa4, E = cellsa2 })
cellsa4:link({ N = Bath1B, W = cellsa5, E = cellsa3 })
cellsa5:link({ W = cellsa6, E = cellsa4 })
cellsa6:link({ E = cellsa5 })

mad = cellsa2:addSpot( Spot(SOUTH, {947, 1047}) )
mad:attach(VIDEO, "vid_mad_cellsa2.ogv", {loop = true})

mad = cellsa5:addSpot( Spot(NORTH, {1036, 1079}) )
mad:attach(VIDEO, "vid_mad_cellsa5.ogv", {loop = true})

mad = cellsa6:addSpot( Spot(NORTH, {406, 824}) )
mad:attach(VIDEO, "vid_mad_scare.ogv")
mad:attach(FUNCTION, 
	function()
		self:play()
	end)
mad:stop()

-- The script must perform a check of the spot, or perhaps
-- export a function from DGControl that does all that. Yeah, better....