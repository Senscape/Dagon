/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGAudio.h"
#include "DGCamera.h"
#include "DGConfig.h"
#include "DGConsole.h"
#include "DGControl.h"
#include "DGEffects.h"
#include "DGFont.h"
#include "DGLog.h"
#include "DGMemory.h"
#include "DGRender.h"
#include "DGScript.h"
#include "DGState.h"
#include "DGSystem.h" // Temporary
#include "DGTexture.h"
#include "DGTextureManager.h"
#include "DGVideo.h"
#include "DGWorld.h"


struct {
	int x;
	int y;
	DGBool queued;
	uint32_t color;
	float timer;
	char buffer[DG_MAX_FEEDBACK];
} feedback;

struct {
	DGBool	has_enter_node;
	DGBool	has_leave_node;
	int	enter_node;
	int	leave_node;
	
	DGBool	has_enter_room;
	DGBool	has_leave_room;
	int	enter_room;
	int	leave_room;
	
	DGBool	has_pre_render;
	DGBool	has_post_render;
	int	pre_render;
	int	post_render;
	
	DGBool	has_mouse_move;
	DGBool	has_mouse_button;
	int	mouse_move;
	int	mouse_button;	
} global_events;

struct mouse_coords {
	short x;
	short y;
	DGBool on_spot;
} mouse_coords;

static DGBool running = DG_NO;

void _parse_feedback(const char* text);
DGBool _scan(DGAction** action);
void _update_feedback(void);
void _updateScene(void);

typedef struct {
	DGBool enabled;
	int value;
	char line[DG_MAX_LOGLEN];
} DG_KEY_EVENT;

DG_KEY_EVENT hotkey[12];

// This goes in timer.c
static int timer = 0;
///


void DGControlInitialize() {
	int i;
	
	DGLogInfo(DG_MOD_CONTROL, "Dagon Version %d.%d.%d Build %d", DG_VERSION_MAJOR, DG_VERSION_MINOR, DG_VERSION_RELEASE, DG_VERSION_BUILD);
	DGLogTrace(DG_MOD_CONTROL, "Initializing main controller...");
	DGWorldInitialize();
	DGStateInitialize();
	DGRenderInitialize();
    
	DGTextureManagerInitialize();
	DGCameraInitialize(DGConfig.display_width, DGConfig.display_height);
	DGFontInitialize("veramobd.ttf", 10);
	DGAudioInitialize();
	DGVideoInitialize();
	
	DGLogTrace(DG_MOD_CONTROL, "Initialization complete!");
	
	feedback.queued = DG_NO;
	
	mouse_coords.x = DGConfig.display_width/2;
	mouse_coords.y = DGConfig.display_height/2;
	mouse_coords.on_spot = DG_NO;
	
	running = DG_NO;
	
	for (i = 0; i <= 12; i++)
		hotkey[i].enabled = DG_NO;
    
	if (DGConfig.show_splash) {
		DGStateSet(DG_STATE_SPLASH);
		DGRenderSetSplash();
	}
	
    running = DG_YES;
}

void DGControlShowFeedback(const char* theText) {
	_parse_feedback(theText);
}

DGBool DGControlIsRunning() {
	return running;
}

// TODO: Should merge this functions and use key codes only...

void DGControlProcessCharKey(const char* aKey, DGBool isModified) {
	switch (*aKey) {
		case DG_KEY_ESC:
			running = DG_NO;
			break;
		case DG_KEY_QUOTE:
			DGConsoleToggleState();
			break;
		case DG_KEY_BACKSPACE:
			DGConsoleDeleteChar();
			break;
		case DG_KEY_ENTER:
			DGConsoleExecute();
			break;
        case 'f':
        case 'F':
            if (isModified) {
                if (DGConfig.fullscreen) DGConfig.fullscreen = DG_NO;
                else DGConfig.fullscreen = DG_YES;
                DGSystemToggleFullscreen();
            }
            else DGConsoleInputChar(aKey);
            break;
		default:
			DGConsoleInputChar(aKey);
			break;
	}
}

void DGControlProcessKey(const char aKey) {
	int idx;
	
	switch (aKey) {
		case DG_KEY_F1: idx = 1; break;
		case DG_KEY_F2: idx = 2; break;
		case DG_KEY_F3: idx = 3; break;
		case DG_KEY_F4: idx = 4; break;
		case DG_KEY_F5: idx = 5; break;
		case DG_KEY_F6: idx = 6; break;
		case DG_KEY_F7: idx = 7; break;
		case DG_KEY_F8: idx = 8; break;
		case DG_KEY_F9: idx = 9; break;
		case DG_KEY_F10: idx = 10; break;
		case DG_KEY_F11: idx = 11; break;
		case DG_KEY_F12: idx = 12; break;
		default: idx = 0; break;
	}
	
	if (hotkey[idx].enabled)
		DGScriptDoLine(hotkey[idx].line);
}

/////////////////////////////////////////////////////////////

void DGControlProcessMouse(short xPosition, short yPosition, DGBool isButtonPressed) {
	DGBool foundAction = DG_NO; // Let's try to avoid this...
	DGAction* action;
    DGObject* audio;
	
	if (global_events.has_mouse_move) 
		DGScriptCallback(global_events.mouse_move);
	
	mouse_coords.x = xPosition;
	mouse_coords.y = yPosition;
	
	// TODO: Only under certain states
	if (DGStateCurrent() != DG_STATE_NODE)
		return;
	
	DGCameraPan(xPosition, yPosition);
	
	foundAction = _scan(&action);
	
	if (isButtonPressed) {
		if (global_events.has_mouse_button) 
			DGScriptCallback(global_events.mouse_button);
		
		if (foundAction) {
			//_updateScene(); // Draw without the cursor
			//dg_render_copy_scene();
			
			switch (action->type) {
				case DG_ACTION_CUSTOM:
					DGScriptCallback(action->custom_handler);
					break;
				case DG_ACTION_FEEDBACK:
					_parse_feedback(action->feedback);
                    if (strcmp(action->feedback_audio, "") != 0) {
                        audio = DGAudioNew(action->feedback_audio, DG_NO);
                        DGAudioLoad(audio);
                        DGAudioPlay(audio);
                    }
					break;
				case DG_ACTION_SWITCH:
					// Uncomment to take snapshots per click
					//dg_control_snap();
					DGControlSwitchTo(action->target); // Ugly
					break;
			}
		}
	}
}

void DGControlRegisterHotkey(int theKey, const char* luaCommandToExecute) {
	int idx;
	
	switch (theKey) {
		case DG_KEY_F1: idx = 1; break;
		case DG_KEY_F2: idx = 2; break;
		case DG_KEY_F3: idx = 3; break;
		case DG_KEY_F4: idx = 4; break;
		case DG_KEY_F5: idx = 5; break;
		case DG_KEY_F6: idx = 6; break;
		case DG_KEY_F7: idx = 7; break;
		case DG_KEY_F8: idx = 8; break;
		case DG_KEY_F9: idx = 9; break;
		case DG_KEY_F10: idx = 10; break;
		case DG_KEY_F11: idx = 11; break;
		case DG_KEY_F12: idx = 12; break;
		default: idx = 0; break;
	}
	
	hotkey[idx].enabled = DG_YES;
	hotkey[idx].value = theKey;
	strcpy(hotkey[idx].line, luaCommandToExecute);
}

void DGControlRegisterGlobalHandler(unsigned forEvent, int handlerForLua) {
	switch (forEvent) {
		case DG_EVENT_ENTER_NODE:
			global_events.enter_node = handlerForLua;
			global_events.has_enter_node = DG_YES;
			break;
		case DG_EVENT_LEAVE_NODE:
			global_events.leave_node = handlerForLua;
			global_events.has_leave_node = DG_YES;
			break;
		case DG_EVENT_ENTER_ROOM:
			global_events.enter_room = handlerForLua;
			global_events.has_enter_room = DG_YES;
			break;
		case DG_EVENT_LEAVE_ROOM:
			global_events.leave_room = handlerForLua;
			global_events.has_leave_room = DG_YES;
			break;
		case DG_EVENT_PRE_RENDER:
			global_events.pre_render = handlerForLua;
			global_events.has_pre_render = DG_YES;
			break;
		case DG_EVENT_POST_RENDER:
			global_events.post_render = handlerForLua;
			global_events.has_post_render = DG_YES;
			break;
		case DG_EVENT_MOUSE_BUTTON:
			global_events.mouse_button = handlerForLua;
			global_events.has_mouse_button = DG_YES;
			break;			
		case DG_EVENT_MOUSE_MOVE:
			global_events.mouse_move = handlerForLua;
			global_events.has_mouse_move = DG_YES;
			break;			
	}
}

void DGControlTerminate() {
	DGLogTrace(DG_MOD_CONTROL, "Finalizing...");
	
	DGAudioTerminate();
    DGVideoTerminate();
	DGWorldTerminate();
	DGFontClean();
	DGTextureManagerTerminate();
	DGRenderTerminate();
	DGReportMem();
}

void DGControlSleep(int forMilliseconds) {
	timer = forMilliseconds;
	DGStateSet(DG_STATE_TIMER);
}

void DGControlTakeSnapshot() {
	_updateScene();
	DGRenderCopyScene();
	DGRenderSaveScene();
}

void DGControlSwitchTo(DGObject* theTarget) {
	static DGBool first = DG_YES; // Fix this, please...
	DGObject* spot;
	DGObject* node;
	
    // Ugly workaround for now
    if (DGStateCurrent() == DG_STATE_NODE) {
        _updateScene(); // Draw without the cursor
        DGRenderCopyScene();
    }
	
	if (global_events.has_leave_node) 
		DGScriptCallback(global_events.leave_node);
	
	while (DGWorldGetSpot(&spot)) {
        if (DGSpotHasAudio(spot)) {
            DGObject* audio = DGSpotAudio(spot);
            DGAudioUnload(audio);
        }
        
		if (DGSpotHasVideo(spot)) {
			DGObject* video = DGSpotVideo(spot);
			DGVideoUnload(video);
            
            // Should not call this if video is set to autoplay... WRONG now
            if (!DGVideoIsLoopable(video))
                DGSpotStop(spot);
		}
	}
	
	if (theTarget) {
		switch (theTarget->type) {
			case DG_OBJECT_NODE:
				DGLogTrace(DG_MOD_CONTROL, "Loading node %s", theTarget->name);
				DGWorldSwitchTo(theTarget);
				break;
			case DG_OBJECT_ROOM:
				if (global_events.has_leave_room) 
					DGScriptCallback(global_events.leave_room);
				
				DGLogTrace(DG_MOD_CONTROL, "Loading room %s", theTarget->name);
				DGWorldSwitchTo(theTarget);
				
				// Hack until we sort out the texture loading process
				theTarget = DGWorldCurrentNode();
				
				if (global_events.has_enter_room) 
					DGScriptCallback(global_events.enter_room);
                
				break;
		}
	}
    
    // Only in debug mode
    node = DGWorldCurrentNode();
    DGSystemSetTitle(node->name);
	
	while (DGWorldGetSpot(&spot)) {
		if (DGSpotHasImage(spot)) {
			DGObject* texture = DGTextureManagerLoad(DGSpotImage(spot));
			if (texture)
				DGSpotSetTexture(spot, texture);
		}
        
        if (DGSpotHasAudio(spot)) {
			DGObject* audio = DGSpotAudio(spot);
			DGAudioLoad(audio);
            
            if (DGSpotIsPlaying(spot)) {
                DGAudioPlay(audio);
            }
        }
		
		if (DGSpotHasVideo(spot)) {
			//DG_OBJECT* video = dg_video_new(dg_spot_video(spot), DG_NO, DG_NO, DG_NO);
			//dg_video_load(video);
			//dg_spot_set_video(spot, video);
			DGObject* video = DGSpotVideo(spot);
			DGVideoLoad(video);
			
			if (!DGSpotHasTexture(spot)) {
				DGFrame* frame = DGVideoGetFrame(video);
				DGObject* texture = DGTextureManagerNew(frame->width, frame->height, frame->bpp);
				if (texture)
					DGSpotSetTexture(spot, texture);
				
				if (DGSpotSizeOfArray(spot) == 2) {
					short* origin = DGSpotArrayOfCoordinates(spot);
					short x = origin[0];
					short y = origin[1];
					short coords [] = { x, y, x + frame->width, y, x + frame->width, y + frame->height, x, y + frame->height };
					DGSpotResize(spot, coords, 8);
				}
			}
		}
	}
	
	if (global_events.has_enter_node) 
		DGScriptCallback(global_events.enter_node);
	
	// Avoid blending the first switch
	if (first) {
		first = DG_NO;
	}
	else {
		DGRenderBlendNextUpdate();
		DGCameraSimulateWalk();
	}
}

void DGControlUpdate() {
	/// Move to timer.c
	// Timer (waiting) is a state, update should call update(last) -> wait
	double duration;
	clock_t currentTime;
	static clock_t lastTime = 0;
	///
    double duration2;
	clock_t currentTime2;
	static clock_t lastTime2 = 0;
    static float nextUpdate = 0;
    ///
    
    if (DGConfig.throb) {
        /// Move to timer.c
        if (!lastTime2) {
            lastTime2 = clock();
            nextUpdate = (float)(rand() % 100);
        }
        currentTime2 = clock();
        duration2 = (double)(currentTime2 - lastTime2) / CLOCKS_PER_SEC * 1000;
        if (duration2 > nextUpdate) {
            int aux = (rand() % 1000) + 1500;
            DGEffectsSetGamma(aux / 1000.0f);
            lastTime2 = 0;
        }
        ///
    }
    else DGEffectsSetGamma(2.5f);
	
    while (running) {
		switch (DGStateCurrent()) {
			case DG_STATE_NODE:
			case DG_STATE_VIDEO: // WRONG                
				DGCameraLoop();
				
				_scan(NULL);
				_updateScene();
				
				// All ortographic projections
				DGRenderBegin(DG_NO);
				
				if (DGCameraState() != DG_CAM_LOOKAT) {
					if (mouse_coords.on_spot)
						//dg_render_with_color(DG_COLOR_BRIGHTCYAN);
						DGRenderSetColor(DG_COLOR_BRIGHTRED);
					else
						DGRenderSetColor(DG_COLOR_DARKGRAY);
					
					DGRenderDrawCursor(mouse_coords.x, mouse_coords.y);
				}
				
				_update_feedback();
				
				DGRenderEnd();
				
				if (DGCameraState() == DG_CAM_LOOKAT_OVER)
					DGScriptResume();
				break;
			case DG_STATE_SPLASH:
				/// Move to timer.c
				if (!lastTime)
					lastTime = clock();
				currentTime = clock();
				duration = (double)(currentTime - lastTime) / CLOCKS_PER_SEC;
				///
				
				if (duration < 2.0f) {
					DGRenderClearScene();
					DGRenderBegin(DG_YES);
					DGCameraSetOrthoView();
					DGRenderDrawSplash();
					DGRenderEnd();
				}
				else {
					DGRenderBlendNextUpdate();
					DGStateSet(DG_STATE_NODE);
					DGScriptResume();
                    DGScriptHack();
					lastTime = 0;
				}
				break;
			case DG_STATE_TIMER:
				/// Move to timer.c
				if (!lastTime)
					lastTime = clock();
				currentTime = clock();
				duration = (double)(currentTime - lastTime) / CLOCKS_PER_SEC * 1000;
				///
				
				// Patch to avoid rotating
				DGCameraPan(DGConfig.display_width / 2, DGConfig.display_height / 2);
				
				DGCameraLoop();
				_updateScene();
				
				// All ortographic projections
				DGRenderBegin(DG_NO);
				_update_feedback();
				DGRenderEnd();
				
				if (duration > timer) {
					timer = 0;
					lastTime = 0;
					DGStateReturn();
					DGScriptResume();
				}
				break;
		}
		
		if (!DGConfig.multithread)
			break;
    }
}

void _parse_feedback(const char* text) {
	// This goes elsewhere
	const int margin = 10;
	const int size = 10;
	const int max_chars = DGConfig.display_width / size;
	
	int len = strlen(text);
	strcpy(feedback.buffer, text);
	
	if (len > max_chars) {
		char* p = feedback.buffer;
		int split = len / max_chars;
		int even = len / (split + 1);
		
		int c, s;
		for (s = 0; s < split; s++) {
			for (c = 0; c < even; c++)
				p++;
			
			while (*p != ' ')
				p++;
			
			*p = '\n';
		}
		
		feedback.x = (DGConfig.display_width / 2) - ((even / 2) * size) + margin;
		feedback.y = DGConfig.display_height - ((split + 1) * size) - margin;
	}
	else {
		feedback.x = (DGConfig.display_width / 2) - ((len / 2) * size) + margin;
		feedback.y = DGConfig.display_height - size - margin;
	}
	
	feedback.queued = DG_YES;
	feedback.timer = (float)(len * 5); // Should be configurable
	feedback.color = DG_COLOR_WHITE;
}

DGBool _scan(DGAction** action) {
	int color;
	DGBool foundAction = DG_NO; // Let's try to avoid this...
	DGObject* spot;
	mouse_coords.on_spot = DG_NO;
	
	// TODO: Completely broken in multithread!!
	// TODO: Process only if cursor within client window
	DGRenderClearScene();
	DGCameraSetViewport(DGConfig.display_width, DGConfig.display_height);	
	DGCameraUpdate();
	DGRenderBegin(DG_NO);
	
	while (DGWorldGetSpot(&spot)) {
		if (DGSpotHasColor(spot) && DGSpotIsEnabled(spot) && !foundAction) {
			DGRenderSetColor(DGSpotColor(spot));
			DGRenderDrawPolygon(DGSpotArrayOfCoordinates(spot), 
								  DGSpotSizeOfArray(spot), 
								  DGSpotFace(spot), 
								  DG_NO);
			
			color = DGRenderTestColor(mouse_coords.x, mouse_coords.y);
			if (color) {
				DGAction* aux;
				if (DGWorldGetAction(color, &aux)) {
					if (action) {
						*action = aux;
						DGWorldSetCurrentSpot(spot);
					}
					foundAction = DG_YES;
					mouse_coords.on_spot = DG_YES;
					// TODO: Must finish the iteration before breaking!
					//break;
				}
				else {
					//dg_log_warning(DG_MOD_CONTROL, "Action not found for color %d", color);
				}
				
			}
		}
	}
	DGRenderEnd();
	
	return foundAction;
}

void _update_feedback() {
	// TODO: Must revise this camera thing... too many changes
	if (feedback.queued) {
		DGCameraSetOrthoView();
		DGRenderSetColor(feedback.color);
		DGFontPrint((float)feedback.x, (float)feedback.y, feedback.buffer);
		DGCameraSetPerspectiveView();
		
		if (feedback.timer <= 0) {
			feedback.color -= 0x05000000;
			
			if (feedback.color <= 0x01000000)
				feedback.queued = DG_NO;
		}
		else
			feedback.timer--;
	}
}

void _updateScene() {
	DGObject* spot;
    
	if (!DGConfig.show_spots)
		DGRenderClearScene();
    
	DGCameraSetViewport(DGConfig.display_width, DGConfig.display_height);
	DGCameraUpdate();
	
	if (global_events.has_pre_render) 
		DGScriptCallback(global_events.pre_render);
	
	DGRenderBegin(DG_YES);
	while (DGWorldGetSpot(&spot)) {
		if (DGSpotIsEnabled(spot)) {
			if (DGSpotHasTexture(spot)) {
				DGTextureManagerBind(DGSpotTexture(spot));
                
                /*if (dg_spot_has_audio(spot)) {
                    DG_OBJECT* audio = dg_spot_audio(spot);
                    
                    if (dg_spot_is_playing(spot)) {
                        if (!dg_audio_is_playing(audio)) {
                            dg_audio_play(audio);
                        }
                    }
                }*/
				
				if (DGSpotHasVideo(spot)) {
					DGObject* video = DGSpotVideo(spot);
					
					if (DGSpotIsPlaying(spot)) {
						DGVideoPlay(video); // Unnecessary
						
                        // Create a state machine for the video...
                        
						// THIS IS PATCHY AND NOT GENERIC ENOUGH
						// Should not be tested always and only works for nodes
						if (DGVideoIsSynced(video)) {
							//dg_state_set(DG_STATE_VIDEO);
							if (DGVideoIsReset(video)) {
								DGScriptResume(); // Must do this once
							}
						}
						
						if (DGVideoHasFrame(video)) {
							DGRenderShowCutscene(video);	
						}
                        
                        // TODO: Clean code
                        if (DGVideoIsFrameReady(video)) {
                            DGRenderDrawPolygon(DGSpotArrayOfCoordinates(spot), 
                                                  DGSpotSizeOfArray(spot), 
                                                  DGSpotFace(spot), 
                                                  DG_NO);
                        }
					}
					else {
						//dg_video_stop(video); // Unnecessary
						//is_first_frame = DG_YES;
					}

				}
				else {
					DGRenderDrawPolygon(DGSpotArrayOfCoordinates(spot), 
										  DGSpotSizeOfArray(spot), 
										  DGSpotFace(spot), 
										  DG_NO);
				}
			}
		}
	}
	
    
	DGRenderEnd();
	DGRenderUpdateScene();
    
	if (global_events.has_post_render) 
		DGScriptCallback(global_events.post_render);
}
