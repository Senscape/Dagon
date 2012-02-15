room 'Cafeteria'

spot = cafeteria1:addSpot( Spot(EAST, {512, 512, 1536, 512, 1536, 2048, 512, 2048}) )
spot2 = cafeteria1:addSpot( Spot(WEST, {512, 512, 1536, 512, 1536, 2048, 512, 2048}) )
spot:attach(CUSTOM, function() self:toggle() end)
spot2:attach(CUSTOM, function() self:toggle() end)
switch(Cafeteria)
