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

intro = Overlay("Intro")  
logo = Image("img_logo.png")
logo:scale(0.6)

credit1 = Image("img_credit1.png")
credit1:disable()
w, h = credit1:size()
credit1:setPosition(50, (config.displayHeight / 2) - (h / 2))
	
credit2 = Image("img_credit2.png")
credit2:disable()
w, h = credit2:size()
credit2:setPosition(config.displayWidth - w - 50, 
	(config.displayHeight / 2) - (h / 2))

credit3 = Image("img_credit3.png")
credit3:disable()
w, h = credit3:size()
credit3:setPosition(50, (config.displayHeight / 2) - (h / 2))

credit4 = Image("img_credit4.png")
credit4:disable()
w, h = credit4:size()
credit4:setPosition((config.displayWidth / 2) - (w / 2), 
	(config.displayHeight / 2) - (h / 2))

intro:addImage(credit1)
intro:addImage(credit2)
intro:addImage(credit3)
intro:addImage(credit4)
			
intro:addImage(logo)
intro:show()

timerLoop = 1

timerID = startTimer(5, 
	function()
		if (timerLoop == 1) then
			logo:fadeOut()
			cafeteria1:fadeIn()
			icon:fadeIn()
		elseif (timerLoop == 2) then	
			credit1:fadeIn()
		elseif (timerLoop == 4) then	
			credit1:fadeOut()
			credit2:fadeIn()
		elseif (timerLoop == 6) then	
			credit2:fadeOut()
			credit3:fadeIn()
		elseif (timerLoop == 8) then	
			credit3:fadeOut()
			credit4:fadeIn()			
		elseif (timerLoop == 10) then
			credit4:fadeOut()
		elseif (timerLoop == 11) then			
			stopTimer(timerID)
		end
		
		timerLoop = timerLoop + 1
	end)


register(POST_RENDER, 
	function()
		logo:scale(1.0015)
		w, h = logo:size()
		logo:setPosition((config.displayWidth / 2) - (w / 2), 
			(config.displayHeight / 2) - (h / 2))
	
		journalAnimateIcon()
		
		if jCurrentState == jStates.OPENING then
			x, y = jOpened:position()
			if y < jClosedOffset then
				jCurrentState = jStates.OPENED
			else
				jOpened:move(0, -jOpeningSpeed)
			end
		elseif jCurrentState == jStates.CLOSING then
			x, y = jOpened:position()
			if y > jOpenedOffset then
				jOpened:hide()		
				jCurrentState = jStates.CLOSED
			else
				jOpened:move(0, jOpeningSpeed)
			end
		end							
	end)

cafeteria1:disable()
switch(Cafeteria)