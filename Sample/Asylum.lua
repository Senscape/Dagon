room 'Cafeteria'

config.mute = true
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

journalStates = {
  IDLE = 100,
  OPENING = 101,
  CLOSING = 102
}

journalState = journalStates.IDLE

journalClosed = Overlay("Journal Closed")
journalOpened = Overlay("Journal Opened")

journalPositionX = config.displayWidth - 430
journalPositionY = config.displayHeight - 500

journalIconSize = 200
journalSpeed = 10

icon = Button(config.displayWidth - journalIconSize, config.displayHeight - journalIconSize,
				journalIconSize, journalIconSize)
icon:setAction(CUSTOM, function()
							journalOpened:show()				
							journalState = journalStates.OPENING						
					   end)
icon:setImage("icon_journal.tga")

journalClosed:addButton(icon)
journalClosed:show()

background = Button(journalPositionX, journalPositionY, 450, 675)
background:setAction(CUSTOM, function()
							journalState = journalStates.CLOSING							
					   end)
					   
background:setImage("bg_journal.tga")
background:setPosition(850, 800)

journalOpened:addButton(background)

register(POST_RENDER, function()
							if journalState == journalStates.OPENING then
								background:move(0, -journalSpeed)
								x, y = background:position()
								if y < journalPositionY then
									journalState = journalStates.IDLE
								end
							elseif journalState == journalStates.CLOSING then
								background:move(0, journalSpeed)
								x, y = background:position()
								if y > config.displayHeight then
									journalOpened:hide()		
									journalState = journalStates.IDLE
								end
							end							
					  end)
					  
register(RESIZE, function()
					journalPositionX = config.displayWidth - 430
					journalPositionY = config.displayHeight - 500
				 end)					  

switch(Cafeteria)
