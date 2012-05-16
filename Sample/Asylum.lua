room 'Cafeteria'

setFont(FEED, "dislexia.ttf", 18)

daniel0 = Audio("trck_daniel_layer0.ogg", {loopable = true})
daniel1 = Audio("trck_daniel_layer1.ogg", {loopable = true})

-- Remove tags
-- Add sync to audio
-- Remove convenience methods from attach

Cafeteria:addAudio(daniel0)

CorridorB:addAudio(daniel0)
CorridorB:addAudio(daniel1)

spot = cafeteria3:addSpot( Spot(WEST, {1536, 1164, 1625, 1164, 1625, 1290, 1536, 1290}) )
alarm = spot:attach(AUDIO, Audio("sfx_alarm.ogg", {loopable = true}) )
spot:attach(CUSTOM, function() 
						if self:isPlaying() == false then 
							self:play() 
						else
							self:stop()
						end
					end)

spot = cafeteria2:addSpot( Spot(NORTH, {1024, 1024}) )
spot:attach(AUDIO, alarm, {volume = 50})

spot = cafeteria1:addSpot( Spot(NORTH, {1024, 1024}) )
spot:attach(AUDIO, alarm, {volume = 25})

switch(Cafeteria)
