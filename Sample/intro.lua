Intro = {}
Intro.__index = Intro

function Intro.create()
	local self = { -- our new object
		intro,
		logo,
		credit1,
		credit2,
		credit3,
		credit4,
		
		timerID,
		timerLoop = 1,
		
		running = false
  }
  
	setmetatable(self, Intro)
	 
	self.logo = Image("img_logo.png")
	self.logo:scale(0.5)

	self.credit1 = Image("img_credit1.png")
	self.credit1:disable()
	
	self.credit2 = Image("img_credit2.png")
	self.credit2:disable()

	self.credit3 = Image("img_credit3.png")
	self.credit3:disable()

	self.credit4 = Image("img_credit4.png")
	self.credit4:disable()
	
	self.intro = Overlay("Intro") 
	self.intro:addImage(self.logo)
	self.intro:addImage(self.credit1)
	self.intro:addImage(self.credit2)
	self.intro:addImage(self.credit3)
	self.intro:addImage(self.credit4)
	
	self:resize()
		
	return self
end

function Intro:start()
	cafeteria1:disable()
	daniel0:stop()
	cursor.disable()
	self.intro:enable()
	play("sfx_thump.ogg")

	self.timerID = startTimer(4, 
		function()
			if (self.timerLoop == 1) then
				self.logo:fadeOut()
				cafeteria1:fadeIn()
				daniel0:play()
			elseif (self.timerLoop == 2) then	
				self.credit1:fadeIn()
				cursor.fadeIn()
			elseif (self.timerLoop == 4) then	
				self.credit1:fadeOut()
				self.credit2:fadeIn()
			elseif (self.timerLoop == 6) then	
				self.credit2:fadeOut()
				self.credit3:fadeIn()
			elseif (self.timerLoop == 8) then	
				self.credit3:fadeOut()
				self.credit4:fadeIn()			
			elseif (self.timerLoop == 10) then
				self.credit4:fadeOut()
			elseif (self.timerLoop == 11) then			
				stopTimer(self.timerID)
			end
			
			self.timerLoop = self.timerLoop + 1
		end)
		
		running = true
end

function Intro:cancel()
	self.intro:disable()
	cafeteria1:fadeIn()
	cursor.fadeIn()
	stopTimer(self.timerID)
	
	running = false
end

function Intro:resize()
	local w, h = 0
	
	w, h = self.logo:size()
	self.logo:setPosition((config.displayWidth / 2) - (w / 2), 
			(config.displayHeight / 2) - (h / 2))
	
	w, h = self.credit1:size()
	self.credit1:setPosition(50, (config.displayHeight / 2) - (h / 2))
		
	w, h = self.credit2:size()
	self.credit2:setPosition(config.displayWidth - w - 50, 
		(config.displayHeight / 2) - (h / 2))
	
	w, h = self.credit3:size()
	self.credit3:setPosition(50, (config.displayHeight / 2) - (h / 2))
	
	w, h = self.credit4:size()
	self.credit4:setPosition((config.displayWidth / 2) - (w / 2), 
		(config.displayHeight / 2) - (h / 2))
end

function Intro:update()
	if running == true then
		local w, h = 0
	
		self.logo:scale(1.0015)
		w, h = self.logo:size()
		self.logo:setPosition((config.displayWidth / 2) - (w / 2), 
				(config.displayHeight / 2) - (h / 2))
	end
end
