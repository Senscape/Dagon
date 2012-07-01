room 'Cafeteria'

require 'intro'
require 'notes'

--config.controlMode = DRAG
config.mute = true

setFont(FEED, "fnt_dislexia.ttf", 20)

daniel0 = Audio("trk_daniel_layer0.ogg", {loopable = true})
daniel0:setFadeSpeed(SLOWEST)

daniel1 = Audio("trk_daniel_layer1.ogg", {loopable = true})
daniel1:setFadeSpeed(SLOWEST)
daniel1:match(daniel0)

daniel2 = Audio("trk_daniel_layer2.ogg", {loopable = true})
daniel2:setFadeSpeed(SLOWEST)
daniel2:match(daniel0)

-- Separate into folders
-- Change image when cursor hovers on journal (without pencil)
-- Integrated readme
-- Hints
-- Add sync to audio
-- Stretch mark animation (on journal)
-- Still cursor while walking?
-- Inherit values from spot

Cafeteria:addAudio(daniel0)
CorridorB:addAudio(daniel0)

Hospital:addAudio(daniel0)
Hospital:addAudio(daniel1)

CellsA:addAudio(daniel0)
CellsA:addAudio(daniel1)
CellsA:addAudio(daniel2)

Bath1B:addAudio(daniel0)
Bath1B:addAudio(daniel1)

Cafeteria:setDefaultFootstep("sfx_step_concrete_l")
CorridorB:setDefaultFootstep("sfx_step_concrete_h")
Hospital:setDefaultFootstep("sfx_step_marble_m")
CellsA:setDefaultFootstep("sfx_step_concrete_h")
Bath1B:setDefaultFootstep("sfx_step_marble_m")

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