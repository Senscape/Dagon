--[[

Module: Dynamic tracks

Copyright (c) 2011 Senscape s.r.l.
All rights reserved.

NOTICE: Senscape permits you to use, modify, and distribute this
file in accordance with the terms of the license agreement accompanying it.

All the assets referenced by this code are the sole property of Senscape s.r.l.
and you are not allowed to re-distribute them in any way.

--]]

-- This module handles the dynamic tracks feature and it's mostly Lua magic

FADE_STEP = 5000 -- milliseconds
MAX_LAYERS = 2

Track = {}
Track.__index = Track

function Track.new(name)
	local trck = {}
	setmetatable(trck, Track)
	
	trck.layer = {}
	trck.activated = {}
	trck.activated[0] = true
	for i = 0, MAX_LAYERS do
		trck.layer[i] = Audio.new(name.."_layer"..i..".ogg", true)
		if i > 0 then
			trck.layer[i]:volume(0)
			trck.activated[i] = false
		end
	end
	
	return trck
end

function Track:active(layers, fade)
	local fade_step = fade
	
	if fade_step == nil then
		fade_step = FADE_STEP
	end
	
	for i = 0, MAX_LAYERS do
		self.activated[i] = false
	end
			
	for index,value in ipairs(layers) do
		if value >= 0 and value <= MAX_LAYERS then
			self.layer[value]:fadein(fade_step)
			self.activated[value] = true
		end
	end
	
	for i = 0, MAX_LAYERS do
		if self.activated[i] == false then
			self.layer[i]:fadeout(fade_step)
		end
	end
end

function Track:fadein(fade)
	local fade_step = fade
	
	if fade_step == nil then
		fade_step = FADE_STEP
	end
	
	for i = 0, MAX_LAYERS do
		if self.activated[i] == true then
			self.layer[i]:fadein(fade_step)
		end
	end
end

function Track:fadeout(fade)
	local fade_step = fade
	
	if fade_step == nil then
		fade_step = FADE_STEP
	end
	
	for i = 0, MAX_LAYERS do
		if self.activated[i] == true then	
			self.layer[i]:fadeout(fade_step)
		end
	end
end

function Track:play()
	for i = 0, MAX_LAYERS do
		self.layer[i]:play()
	end
end

function Track:stop()
	for i = 0, MAX_LAYERS do
		self.layer[i]:stop()
	end
end
