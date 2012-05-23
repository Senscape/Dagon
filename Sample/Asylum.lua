require 'journal'
room 'Cafeteria'

config.controlMode = DRAG
config.mute = true

setFont(FEED, "fnt_dislexia.ttf", 20)

daniel0 = Audio("trk_daniel_layer0.ogg", {loopable = true})
daniel1 = Audio("trk_daniel_layer1.ogg", {loopable = true})

-- Remove tags
-- Add sync to audio
-- Remove convenience methods from attach

Cafeteria:addAudio(daniel0)

CorridorB:addAudio(daniel0)
CorridorB:addAudio(daniel1)			  

switch(Cafeteria)
