jStates = {
  IDLE = 100,
  OPENING = 101,
  CLOSING = 102
}

jCurrentState = jStates.IDLE

jClosed = Overlay("Journal Closed")
jOpened = Overlay("Journal Opened")

jClosedSize = 200 -- It's an icon
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

-- Graphic for the closed journal
icon = Button(jClosedX, jClosedY, jClosedSize, jClosedSize)
icon:setAction(CUSTOM, function()
							jOpened:show()				
							jCurrentState = jStates.OPENING						
					   end)
icon:setImage("icon_journal.tga")

jClosed:addButton(icon)
jClosed:show()

-- Graphics for the opened journal
background = Button(jOpenedX, jOpenedY, jOpenedSizeX, jOpenedSizeY)
background:setAction(CUSTOM, function()
							jCurrentState = jStates.CLOSING							
					   end)
					   
background:setImage("bg_journal.tga")

jOpened:addButton(background)
jOpened:move(0, jOpenedOffset) -- Hide the journal

register(POST_RENDER, function()
							if jCurrentState == jStates.OPENING then
								x, y = jOpened:position()
								if y < jClosedOffset then
									jCurrentState = jStates.IDLE
								else
									jOpened:move(0, -jOpeningSpeed)
								end
							elseif jCurrentState == jStates.CLOSING then
								x, y = jOpened:position()
								if y > jOpenedOffset then
									jOpened:hide()		
									jCurrentState = jStates.IDLE
								else
									jOpened:move(0, jOpeningSpeed)
								end
							end							
					  end)
					  
register(RESIZE, function()
						offsetX = config.displayWidth - previousWidth
						offsetY = config.displayHeight - previousHeight
						
						jClosed:move(offsetX, offsetY)
						jOpened:move(offsetX, offsetY)
						
						jClosedOffset = jClosedOffset + offsetY
						jOpenedOffset = jOpenedOffset + offsetY
						
						previousWidth = config.displayWidth
						previousHeight = config.displayHeight
				 end)