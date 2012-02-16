room 'Cafeteria'

spot = cafeteria1:addSpot( Spot(EAST, {512, 512, 1536, 512, 1536, 2048, 512, 2048}) )
spot:attach(IMAGE, "cafeteria1.tex", 0)
spot:attach(CUSTOM, function() self:toggle() end)

switch(Cafeteria)
