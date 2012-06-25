room 'Cafeteria'

require 'intro'
require 'notes'

--config.controlMode = DRAG
config.mute = true

setFont(FEED, "fnt_dislexia.ttf", 20)

daniel0 = Audio("trk_daniel_layer0.ogg", {loopable = true})
daniel1 = Audio("trk_daniel_layer1.ogg", {loopable = true})

-- Separate into folders
-- Change image when cursor hovers on journal (without pencil)
-- Integrated readme
-- Hints
-- Remove tags
-- Add sync to audio
-- Remove convenience methods from attach
-- Stretch mark animation (on journal)

Cafeteria:addAudio(daniel0)

CorridorB:addAudio(daniel0)
CorridorB:addAudio(daniel1)

intro = Intro.create()

register(POST_RENDER, 
	function()
		intro:update()
		journal:animateIcon()
		journal:update()					
	end)
	
register(RESIZE, 
	function()
		intro:resize()
		journal:resize()
	end)

loadCursor(NORMAL, "cur_normal.tga")
loadCursor(DRAGGING, "cur_dragging.tga")
loadCursor(LEFT, "cur_left.tga")
loadCursor(RIGHT, "cur_right.tga")
loadCursor(UP, "cur_up.tga")
loadCursor(DOWN, "cur_down.tga")
loadCursor(UP_LEFT, "cur_upleft.tga")
loadCursor(UP_RIGHT, "cur_upright.tga")
loadCursor(DOWN_LEFT, "cur_downleft.tga")
loadCursor(DOWN_RIGHT, "cur_downright.tga")
loadCursor(FORWARD, "cur_forward.tga")
loadCursor(BACKWARD, "cur_backward.tga")
loadCursor(USE, "cur_use.tga")
loadCursor(LOOK, "cur_look.tga")
loadCursor(TALK, "cur_talk.tga")
loadCursor(CUSTOM, "cur_custom.tga")

switch(Cafeteria)

-- Special effects are always cleared up when switching
--intro:start()