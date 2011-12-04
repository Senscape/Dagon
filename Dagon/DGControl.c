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
	DG_BOOL queued;
	uint32_t color;
	float timer;
	char buffer[DG_MAX_FEEDBACK];
} feedback;

struct {
	DG_BOOL	has_enter_node;
	DG_BOOL	has_leave_node;
	int	enter_node;
	int	leave_node;
	
	DG_BOOL	has_enter_room;
	DG_BOOL	has_leave_room;
	int	enter_room;
	int	leave_room;
	
	DG_BOOL	has_pre_render;
	DG_BOOL	has_post_render;
	int	pre_render;
	int	post_render;
	
	DG_BOOL	has_mouse_move;
	DG_BOOL	has_mouse_button;
	int	mouse_move;
	int	mouse_button;	
} global_events;

struct mouse_coords {
	short x;
	short y;
	DG_BOOL on_spot;
} mouse_coords;

static DG_BOOL running = DG_NO;

void _parse_feedback(const char* text);
DG_BOOL _scan(DG_ACTION** action);
void _update_feedback(void);
void _updateScene(void);

typedef struct {
	DG_BOOL enabled;
	int value;
	char line[DG_MAX_LOGLEN];
} DG_KEY_EVENT;

DG_KEY_EVENT hotkey[12];

// This goes in timer.c
static int timer = 0;
///


void dg_control_init() {
	int i;
	
	dg_log_info(DG_MOD_CONTROL, "Dagon Version %d.%d.%d Build %d", DG_VERSION_MAJOR, DG_VERSION_MINOR, DG_VERSION_RELEASE, DG_VERSION_BUILD);
	dg_log_trace(DG_MOD_CONTROL, "Initializing main controller...");
	dg_world_new();
	dg_state_init();
	dg_render_init();
    
	dg_texture_manager_init();
	dg_camera_init(config.display_width, config.display_height);
	dg_font_init("veramobd.ttf", 10);
	dg_audio_init();
	dg_video_init();
	
	dg_log_trace(DG_MOD_CONTROL, "Initialization complete!");
	
	feedback.queued = DG_NO;
	
	mouse_coords.x = config.display_width/2;
	mouse_coords.y = config.display_height/2;
	mouse_coords.on_spot = DG_NO;
	
	running = DG_NO;
	
	for (i = 0; i <= 12; i++)
		hotkey[i].enabled = DG_NO;
    
	if (config.show_splash) {
		dg_state_set(DG_ST_SPLASH);
		dg_render_set_splash();
	}
	
    running = DG_YES;
}

void dg_control_feedback(const char* text) {
	_parse_feedback(text);
}

DG_BOOL dg_control_is_running() {
	return running;
}

// TODO: Should merge this functions and use key codes only...

void dg_control_process_char(const char* key, DG_BOOL modified) {
	switch (*key) {
		case DG_KEY_ESC:
			running = DG_NO;
			break;
		case DG_KEY_QUOTE:
			dg_console_toggle();
			break;
		case DG_KEY_BACKSPACE:
			dg_console_delete();
			break;
		case DG_KEY_ENTER:
			dg_console_execute();
			break;
        case 'f':
        case 'F':
            if (modified) {
                if (config.fullscreen) config.fullscreen = DG_NO;
                else config.fullscreen = DG_YES;
                dg_system_toggle_fullscreen();
            }
            else dg_console_input(key);
            break;
		default:
			dg_console_input(key);
			break;
	}
}

void dg_control_process_key(const char key) {
	int idx;
	
	switch (key) {
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
		dg_script_doline(hotkey[idx].line);
}

/////////////////////////////////////////////////////////////

void dg_control_process_mouse(short x, short y, DG_BOOL button) {
	DG_BOOL foundAction = DG_NO; // Let's try to avoid this...
	DG_ACTION* action;
    DG_OBJECT* audio;
	
	if (global_events.has_mouse_move) 
		dg_script_callback(global_events.mouse_move);
	
	mouse_coords.x = x;
	mouse_coords.y = y;
	
	// TODO: Only under certain states
	if (dg_state_current() != DG_ST_NODE)
		return;
	
	dg_camera_pan(x, y);
	
	foundAction = _scan(&action);
	
	if (button) {
		if (global_events.has_mouse_button) 
			dg_script_callback(global_events.mouse_button);
		
		if (foundAction) {
			//_updateScene(); // Draw without the cursor
			//dg_render_copy_scene();
			
			switch (action->type) {
				case DG_ACT_CUSTOM:
					dg_script_callback(action->custom_handler);
					break;
				case DG_ACT_FEEDBACK:
					_parse_feedback(action->feedback);
                    if (strcmp(action->feedback_audio, "") != 0) {
                        audio = dg_audio_new(action->feedback_audio, DG_NO);
                        dg_audio_load(audio);
                        dg_audio_play(audio);
                    }
					break;
				case DG_ACT_SWITCH:
					// Uncomment to take snapshots per click
					//dg_control_snap();
					dg_control_switch(action->target); // Ugly
					break;
			}
		}
	}
}

void dg_control_register_hotkey(int value, const char* line) {
	int idx;
	
	switch (value) {
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
	hotkey[idx].value = value;
	strcpy(hotkey[idx].line, line);
}

void dg_control_register_global_handler(unsigned type, int ref) {
	switch (type) {
		case DG_EVENT_ENTER_NODE:
			global_events.enter_node = ref;
			global_events.has_enter_node = DG_YES;
			break;
		case DG_EVENT_LEAVE_NODE:
			global_events.leave_node = ref;
			global_events.has_leave_node = DG_YES;
			break;
		case DG_EVENT_ENTER_ROOM:
			global_events.enter_room = ref;
			global_events.has_enter_room = DG_YES;
			break;
		case DG_EVENT_LEAVE_ROOM:
			global_events.leave_room = ref;
			global_events.has_leave_room = DG_YES;
			break;
		case DG_EVENT_PRE_RENDER:
			global_events.pre_render = ref;
			global_events.has_pre_render = DG_YES;
			break;
		case DG_EVENT_POST_RENDER:
			global_events.post_render = ref;
			global_events.has_post_render = DG_YES;
			break;
		case DG_EVENT_MOUSE_BUTTON:
			global_events.mouse_button = ref;
			global_events.has_mouse_button = DG_YES;
			break;			
		case DG_EVENT_MOUSE_MOVE:
			global_events.mouse_move = ref;
			global_events.has_mouse_move = DG_YES;
			break;			
	}
}

void dg_control_release() {
	dg_log_trace(DG_MOD_CONTROL, "Finalizing...");
	
	dg_audio_terminate();
    dg_video_terminate();
	dg_world_release();
	dg_font_clean();
	dg_texture_manager_release();
	dg_render_release();
	dg_reportmem();
}

void dg_control_sleep(int msecs) {
	timer = msecs;
	dg_state_set(DG_ST_TIMER);
}

void dg_control_snap() {
	_updateScene();
	dg_render_copy_scene();
	dg_render_save_scene();
}

void dg_control_switch(DG_OBJECT* target) {
	static DG_BOOL first = DG_YES; // Fix this, please...
	DG_OBJECT* spot;
	DG_OBJECT* node;
	
    // Ugly workaround for now
    if (dg_state_current() == DG_ST_NODE) {
        _updateScene(); // Draw without the cursor
        dg_render_copy_scene();
    }
	
	if (global_events.has_leave_node) 
		dg_script_callback(global_events.leave_node);
	
	while (dg_world_get_spot(&spot)) {
        if (dg_spot_has_audio(spot)) {
            DG_OBJECT* audio = dg_spot_audio(spot);
            dg_audio_unload(audio);
        }
        
		if (dg_spot_has_video(spot)) {
			DG_OBJECT* video = dg_spot_video(spot);
			dg_video_unload(video);
            
            // Should not call this if video is set to autoplay... WRONG now
            if (!dg_video_has_loop(video))
                dg_spot_stop(spot);
		}
	}
	
	if (target) {
		switch (target->type) {
			case DG_OBJ_NODE:
				dg_log_trace(DG_MOD_CONTROL, "Loading node %s", target->name);
				dg_world_switch(target);
				break;
			case DG_OBJ_ROOM:
				if (global_events.has_leave_room) 
					dg_script_callback(global_events.leave_room);
				
				dg_log_trace(DG_MOD_CONTROL, "Loading room %s", target->name);
				dg_world_switch(target);
				
				// Hack until we sort out the texture loading process
				target = dg_world_current_node();
				
				if (global_events.has_enter_room) 
					dg_script_callback(global_events.enter_room);
                
				break;
		}
	}
    
    // Only in debug mode
    node = dg_world_current_node();
    dg_system_set_title(node->name);
	
	while (dg_world_get_spot(&spot)) {
		if (dg_spot_has_image(spot)) {
			DG_OBJECT* texture = dg_texture_manager_load(dg_spot_image(spot));
			if (texture)
				dg_spot_set_texture(spot, texture);
		}
        
        if (dg_spot_has_audio(spot)) {
			DG_OBJECT* audio = dg_spot_audio(spot);
			dg_audio_load(audio);
            
            if (dg_spot_is_playing(spot)) {
                dg_audio_play(audio);
            }
        }
		
		if (dg_spot_has_video(spot)) {
			//DG_OBJECT* video = dg_video_new(dg_spot_video(spot), DG_NO, DG_NO, DG_NO);
			//dg_video_load(video);
			//dg_spot_set_video(spot, video);
			DG_OBJECT* video = dg_spot_video(spot);
			dg_video_load(video);
			
			if (!dg_spot_has_texture(spot)) {
				DG_FRAME* frame = dg_video_get_frame(video);
				DG_OBJECT* texture = dg_texture_manager_new(frame->width, frame->height, frame->bpp);
				if (texture)
					dg_spot_set_texture(spot, texture);
				
				if (dg_spot_elements(spot) == 2) {
					short* origin = dg_spot_coords(spot);
					short x = origin[0];
					short y = origin[1];
					short coords [] = { x, y, x + frame->width, y, x + frame->width, y + frame->height, x, y + frame->height };
					dg_spot_resize(spot, coords, 8);
				}
			}
		}
	}
	
	if (global_events.has_enter_node) 
		dg_script_callback(global_events.enter_node);
	
	// Avoid blending the first switch
	if (first) {
		first = DG_NO;
	}
	else {
		dg_render_blend();
		dg_camera_walk();
	}
}

void dg_control_update() {
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
    
    if (config.throb) {
        /// Move to timer.c
        if (!lastTime2) {
            lastTime2 = clock();
            nextUpdate = (float)(rand() % 100);
        }
        currentTime2 = clock();
        duration2 = (double)(currentTime2 - lastTime2) / CLOCKS_PER_SEC * 1000;
        if (duration2 > nextUpdate) {
            int aux = (rand() % 1000) + 1500;
            dg_effects_set_gamma(aux / 1000.0f);
            lastTime2 = 0;
        }
        ///
    }
    else dg_effects_set_gamma(2.5f);
	
    while (running) {
		switch (dg_state_current()) {
			case DG_ST_NODE:
			case DG_ST_VIDEO: // WRONG                
				dg_camera_loop();
				
				_scan(NULL);
				_updateScene();
				
				// All ortographic projections
				dg_render_begin(DG_NO);
				
				if (dg_camera_state() != DG_CAM_LOOKAT) {
					if (mouse_coords.on_spot)
						//dg_render_with_color(DG_COLOR_BRIGHTCYAN);
						dg_render_with_color(DG_COLOR_BRIGHTRED);
					else
						dg_render_with_color(DG_COLOR_DARKGRAY);
					
					dg_render_draw_cursor(mouse_coords.x, mouse_coords.y);
				}
				
				_update_feedback();
				
				dg_render_end();
				
				if (dg_camera_state() == DG_CAM_LOOKAT_OVER)
					dg_script_resume();
				break;
			case DG_ST_SPLASH:
				/// Move to timer.c
				if (!lastTime)
					lastTime = clock();
				currentTime = clock();
				duration = (double)(currentTime - lastTime) / CLOCKS_PER_SEC;
				///
				
				if (duration < 2.0f) {
					dg_render_clear();
					dg_render_begin(DG_YES);
					dg_camera_set_ortho();
					dg_render_splash();
					dg_render_end();
				}
				else {
					dg_render_blend();
					dg_state_set(DG_ST_NODE);
					dg_script_resume();
                    dg_script_hack();
					lastTime = 0;
				}
				break;
			case DG_ST_TIMER:
				/// Move to timer.c
				if (!lastTime)
					lastTime = clock();
				currentTime = clock();
				duration = (double)(currentTime - lastTime) / CLOCKS_PER_SEC * 1000;
				///
				
				// Patch to avoid rotating
				dg_camera_pan(config.display_width / 2, config.display_height / 2);
				
				dg_camera_loop();
				_updateScene();
				
				// All ortographic projections
				dg_render_begin(DG_NO);
				_update_feedback();
				dg_render_end();
				
				if (duration > timer) {
					timer = 0;
					lastTime = 0;
					dg_state_return();
					dg_script_resume();
				}
				break;
		}
		
		if (!config.multithread)
			break;
    }
}

void _parse_feedback(const char* text) {
	// This goes elsewhere
	const int margin = 10;
	const int size = 10;
	const int max_chars = config.display_width / size;
	
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
		
		feedback.x = (config.display_width / 2) - ((even / 2) * size) + margin;
		feedback.y = config.display_height - ((split + 1) * size) - margin;
	}
	else {
		feedback.x = (config.display_width / 2) - ((len / 2) * size) + margin;
		feedback.y = config.display_height - size - margin;
	}
	
	feedback.queued = DG_YES;
	feedback.timer = (float)(len * 5); // Should be configurable
	feedback.color = DG_COLOR_WHITE;
}

DG_BOOL _scan(DG_ACTION** action) {
	int color;
	DG_BOOL foundAction = DG_NO; // Let's try to avoid this...
	DG_OBJECT* spot;
	mouse_coords.on_spot = DG_NO;
	
	// TODO: Completely broken in multithread!!
	// TODO: Process only if cursor within client window
	dg_render_clear();
	dg_camera_set_viewport(config.display_width, config.display_height);	
	dg_camera_update();
	dg_render_begin(DG_NO);
	
	while (dg_world_get_spot(&spot)) {
		if (dg_spot_has_color(spot) && dg_spot_is_enabled(spot) && !foundAction) {
			dg_render_with_color(dg_spot_color(spot));
			dg_render_draw_poly(dg_spot_coords(spot), 
								  dg_spot_elements(spot), 
								  dg_spot_face(spot), 
								  DG_NO);
			
			color = dg_render_test_color(mouse_coords.x, mouse_coords.y);
			if (color) {
				DG_ACTION* aux;
				if (dg_world_get_action(color, &aux)) {
					if (action) {
						*action = aux;
						dg_world_set_current_spot(spot);
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
	dg_render_end();
	
	return foundAction;
}

void _update_feedback() {
	// TODO: Must revise this camera thing... too many changes
	if (feedback.queued) {
		dg_camera_set_ortho();
		dg_render_with_color(feedback.color);
		dg_font_print((float)feedback.x, (float)feedback.y, feedback.buffer);
		dg_camera_set_perspective();
		
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
	DG_OBJECT* spot;
    
	if (!config.show_spots)
		dg_render_clear();
    
	dg_camera_set_viewport(config.display_width, config.display_height);
	dg_camera_update();
	
	if (global_events.has_pre_render) 
		dg_script_callback(global_events.pre_render);
	
	dg_render_begin(DG_YES);
	while (dg_world_get_spot(&spot)) {
		if (dg_spot_is_enabled(spot)) {
			if (dg_spot_has_texture(spot)) {
				dg_texture_manager_bind(dg_spot_texture(spot));
                
                /*if (dg_spot_has_audio(spot)) {
                    DG_OBJECT* audio = dg_spot_audio(spot);
                    
                    if (dg_spot_is_playing(spot)) {
                        if (!dg_audio_is_playing(audio)) {
                            dg_audio_play(audio);
                        }
                    }
                }*/
				
				if (dg_spot_has_video(spot)) {
					DG_OBJECT* video = dg_spot_video(spot);
					
					if (dg_spot_is_playing(spot)) {
						dg_video_play(video); // Unnecessary
						
                        // Create a state machine for the video...
                        
						// THIS IS PATCHY AND NOT GENERIC ENOUGH
						// Should not be tested always and only works for nodes
						if (dg_video_is_synced(video)) {
							//dg_state_set(DG_ST_VIDEO);
							if (dg_video_has_reset(video)) {
								dg_script_resume(); // Must do this once
							}
						}
						
						if (dg_video_has_frame(video)) {
							dg_render_cutscene(video);	
						}
                        
                        // TODO: Clean code
                        if (dg_video_is_frame_ready(video)) {
                            dg_render_draw_poly(dg_spot_coords(spot), 
                                                  dg_spot_elements(spot), 
                                                  dg_spot_face(spot), 
                                                  DG_NO);
                        }
					}
					else {
						//dg_video_stop(video); // Unnecessary
						//is_first_frame = DG_YES;
					}

				}
				else {
					dg_render_draw_poly(dg_spot_coords(spot), 
										  dg_spot_elements(spot), 
										  dg_spot_face(spot), 
										  DG_NO);
				}
			}
		}
	}
	
    
	dg_render_end();
	dg_render_update_scene();
    
	if (global_events.has_post_render) 
		dg_script_callback(global_events.post_render);
}
