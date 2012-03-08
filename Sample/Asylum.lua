room 'Cafeteria'

spot = cafeteria1:addSpot( Spot(EAST, {512, 512, 1536, 512, 1536, 2048, 512, 2048}) )
spot:attach(FEED, "This is a test. Test.  Test.  Test.  Test.")

timer(10, function() feed("Hello, sailor!") end)

setFont(FEED, "dislexia.ttf", 18)

switch(Cafeteria)
