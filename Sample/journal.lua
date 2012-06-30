jRows = {}

jIconStates = {
  IDLE = 201,
  EXPAND = 202,
  CONTRACT = 203
}

jStates = {
  OPENING = 101,
  CLOSING = 102,
  OPENED = 103,
  CLOSED = 104
}

Journal = {}
Journal.__index = Journal

function Journal.create()
	local self = { -- our new object
		currentState = jStates.CLOSED,
	
		iconAnimLoop = 0,
		iconAnimState = jIconStates.IDLE,
		
		closed,
		opened,
		icon,
		background,

		closedSize = 180, -- It's an icon
		openedSizeX = 450,
		openedSizeY = 675,
		closedOffset = 0,
		openedOffset = 500,
		openingSpeed = 10,
		
		closedX = 0,
		closedY = 0,
		openedX = 0, -- To cover the icon
		openedY = 0,
		
		previousWidth = config.displayWidth,
		previousHeight = config.displayHeight
  }
  
	setmetatable(self, Journal) -- make Account handle lookup
	
	local _toggle = function()
		if self.currentState == jStates.CLOSED then
			self.opened:enable()
			play("sfx_journal_open.ogg")
			self.currentState = jStates.OPENING
		elseif self.currentState == jStates.OPENED then
			play("sfx_journal_close.ogg")
			self.currentState = jStates.CLOSING		
		end
	end
	
	self.closed = Overlay("Journal Closed")
	self.opened = Overlay("Journal Opened")
	
	self:resize()
	
	-- Graphic for the closed journal
	self.icon = Button(self.closedX, self.closedY, self.closedSize, self.closedSize)
	self.icon:setAction(FUNCTION, _toggle)
	self.icon:setCursor(CUSTOM)
	self.icon:setImage("img_journal_icon.tga")

	self.closed:addButton(self.icon)
	self.closed:enable()

	-- Graphics for the opened journal
	self.background = Button(self.openedX, self.openedY, self.openedSizeX, self.openedSizeY)
	self.background:setAction(FUNCTION, _toggle)
	self.background:setCursor(BACKWARD)			   
	self.background:setImage("img_journal_bg.tga")
	
	self.opened:addButton(self.background)
	self.opened:enable()
	self.opened:move(0, self.openedOffset) -- Hide the journal
		
	return self
end

function Journal:addEntry(e, f)
	local i = table.getn(jRows) -- Get length of table
	i = i + 1
	x, y = self.opened:position()
	jRows[i] = Button(self.openedX + 50, y + self.openedY + 60 + (i * 40), string.len(e) * 13, 32)
	jRows[i]:setAction(FUNCTION, f)	
	jRows[i]:setCursor(CUSTOM)
	jRows[i]:setFont("fnt_quikhand.ttf", 24)
	jRows[i]:setTextColor(0xBB000000)
	jRows[i]:setText(e)
	self.opened:addButton(jRows[i])
	self.iconAnimLoop = 2
	--play("sfx_journal_write.ogg")
end

function Journal:markEntry(e)
	for i,v in pairs(jRows) do
  	if v:text() == e then
    		jRows[i]:setImage("img_completed.tga")
    		self.iconAnimLoop = 2
    		play("sfx_journal_erase.ogg")
    	return
  	end
	end
end

function Journal:tagEntry(e)
	for i,v in pairs(jRows) do
  	if v:text() == (e) then
  		x, y = self.opened:position()
    	tag = Image("img_tag.tga")
			tag:setPosition(self.openedX + 20, y + self.openedY + 60 + (i * 40))
			self.opened:addImage(tag)
			self.iconAnimLoop = 2
			play("sfx_journal_tag.ogg")
    	return
  	end
	end
end

function Journal:animateIcon()
	if self.iconAnimState == jIconStates.IDLE then
		self.iconAnimState = jIconStates.EXPAND
	elseif self.iconAnimState == jIconStates.EXPAND then
		if self.closedSize < (180 + 20) then
			self.closedSize = self.closedSize + 1
			self.closedX = self.closedX - 1
			self.closedY = self.closedY - 1
		else
			self.iconAnimState = jIconStates.CONTRACT
		end
		
		self.icon:setPosition(self.closedX, self.closedY)		
		self.icon:setSize(self.closedSize, self.closedSize)
	elseif self.iconAnimState == jIconStates.CONTRACT then
		if self.closedSize > 180 then
			self.closedSize = self.closedSize - 1
			self.closedX = self.closedX + 1
			self.closedY = self.closedY + 1
		else
			self.iconAnimState = jIconStates.EXPAND
			self.iconAnimLoop = self.iconAnimLoop - 1
		end
		
		self.icon:setPosition(self.closedX, self.closedY)		
		self.icon:setSize(self.closedSize, self.closedSize)	
	end
	
	if self.iconAnimLoop == 0 then
		self.iconAnimState = jIconStates.IDLE
	end
end	

function Journal:resize()
		offsetX = config.displayWidth - self.previousWidth
		offsetY = config.displayHeight - self.previousHeight
						
		self.closed:move(offsetX, offsetY)
		self.opened:move(offsetX, offsetY)
						
		self.closedOffset = self.closedOffset + offsetY
		self.openedOffset = self.openedOffset + offsetY
		
		self.closedX = config.displayWidth - self.closedSize
		self.closedY = config.displayHeight - self.closedSize
		self.openedX = config.displayWidth - self.openedSizeX + 20 -- To cover the icon
		self.openedY = config.displayHeight - self.openedOffset		
						
		self.previousWidth = config.displayWidth
		self.previousHeight = config.displayHeight
end

function Journal:update()
		if self.currentState == jStates.OPENING then
			x, y = self.opened:position()
			if y < self.closedOffset then
				self.currentState = jStates.OPENED
			else
				self.opened:move(0, -self.openingSpeed)
			end
		elseif self.currentState == jStates.CLOSING then
			x, y = self.opened:position()
			if y > self.openedOffset then
				self.opened:disable()		
				self.currentState = jStates.CLOSED
			else
				self.opened:move(0, self.openingSpeed)
			end
		end		
end
