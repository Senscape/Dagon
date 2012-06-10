jEntries = {
	TUNNEL = "'Tunnel of the Damned'",
	HOSPITAL = "Hospital",
	KEY_MALE = "Key to Male Cells"
}

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

jIconLoop = 0
jIconState = jIconStates.IDLE
jCurrentState = jStates.CLOSED

jClosed = Overlay("Journal Closed")
jOpened = Overlay("Journal Opened")

jClosedSize = 180 -- It's an icon
jOpenedSizeX = 450
jOpenedSizeY = 675
jClosedOffset = 0
jOpenedOffset = 500
jOpeningSpeed = 10

jClosedX = config.displayWidth - jClosedSize
jClosedY = config.displayHeight - jClosedSize
jOpenedX = config.displayWidth - jOpenedSizeX + 20 -- To cover the icon
jOpenedY = config.displayHeight - jOpenedOffset

previousWidth = config.displayWidth
previousHeight = config.displayHeight

function journalToggle()
	if jCurrentState == jStates.CLOSED then
		jOpened:enable()
		play("sfx_journal_open.ogg")
		jCurrentState = jStates.OPENING
	elseif jCurrentState == jStates.OPENED then
		play("sfx_journal_close.ogg")
		jCurrentState = jStates.CLOSING		
	end
end

-- Graphic for the closed journal
icon = Button(jClosedX, jClosedY, jClosedSize, jClosedSize)
icon:setAction(FUNCTION, journalToggle)
icon:setCursor(CUSTOM)
icon:setImage("img_journal_icon.tga")
icon:disable()

jClosed:addButton(icon)
jClosed:enable()

-- Graphics for the opened journal
background = Button(jOpenedX, jOpenedY, jOpenedSizeX, jOpenedSizeY)
background:setAction(FUNCTION, journalToggle)
background:setCursor(BACKWARD)			   
background:setImage("img_journal_bg.tga")

function journalAddEntry(e, f)
	local i = table.getn(jRows) -- Get length of table
	i = i + 1
	x, y = jOpened:position()
	jRows[i] = Button(jOpenedX + 50, y + jOpenedY + 60 + (i * 40), string.len(e) * 13, 32)
	jRows[i]:setAction(FUNCTION, f)	
	jRows[i]:setCursor(CUSTOM)
	jRows[i]:setFont("fnt_quikhand.ttf", 24)
	jRows[i]:setTextColor(0xBB000000)
	jRows[i]:setText(e)
	jOpened:addButton(jRows[i])
	jIconLoop = 2
	play("sfx_journal_write.ogg")
end

function journalMarkEntry(e)
	for i,v in pairs(jRows) do
  	if v:text() == e then
    		jRows[i]:setImage("img_completed.tga")
    		jIconLoop = 2
    		play("sfx_journal_erase.ogg")
    	return
  	end
	end
end

function journalTagEntry(e)
	for i,v in pairs(jRows) do
  	if v:text() == (e) then
  		x, y = jOpened:position()
    	tag = Image("img_tag.tga")
			tag:setPosition(jOpenedX + 20, y + jOpenedY + 60 + (i * 40))
			jOpened:addImage(tag)
			jIconLoop = 2
			play("sfx_journal_tag.ogg")
    	return
  	end
	end
end

jOpened:addButton(background)
jOpened:move(0, jOpenedOffset) -- Hide the journal

journalAddEntry(jEntries.TUNNEL, 
	function()
		feed("Scary place.")
	end)
	
journalAddEntry(jEntries.HOSPITAL,
	function()
		feed("I must find the hospital.")
	end)
	
jIconLoop = 0 -- Cancel animation
	
function journalAnimateIcon()
	if jIconState == jIconStates.IDLE then
		jIconState = jIconStates.EXPAND
	elseif jIconState == jIconStates.EXPAND then
		if jClosedSize < (180 + 20) then
			jClosedSize = jClosedSize + 1
			jClosedX = jClosedX - 1
			jClosedY = jClosedY - 1
		else
			jIconState = jIconStates.CONTRACT
		end
		
		icon:setPosition(jClosedX, jClosedY)		
		icon:setSize(jClosedSize, jClosedSize)
	elseif jIconState == jIconStates.CONTRACT then
		if jClosedSize > 180 then
			jClosedSize = jClosedSize - 1
			jClosedX = jClosedX + 1
			jClosedY = jClosedY + 1
		else
			jIconState = jIconStates.EXPAND
			jIconLoop = jIconLoop - 1
		end
		
		icon:setPosition(jClosedX, jClosedY)		
		icon:setSize(jClosedSize, jClosedSize)	
	end
	
	if jIconLoop == 0 then
		jIconState = jIconStates.IDLE
	end
end	
					  
register(RESIZE, 
	function()
		offsetX = config.displayWidth - previousWidth
		offsetY = config.displayHeight - previousHeight
						
		jClosed:move(offsetX, offsetY)
		jOpened:move(offsetX, offsetY)
						
		jClosedOffset = jClosedOffset + offsetY
		jOpenedOffset = jOpenedOffset + offsetY
		
		jClosedX = config.displayWidth - jClosedSize
		jClosedY = config.displayHeight - jClosedSize
		jOpenedX = config.displayWidth - jOpenedSizeX + 20 -- To cover the icon
		jOpenedY = config.displayHeight - jOpenedOffset		
						
		previousWidth = config.displayWidth
		previousHeight = config.displayHeight
	end)
