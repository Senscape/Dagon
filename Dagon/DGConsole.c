/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGCamera.h"
#include "DGCommonGL.h" // Unnecessary
#include "DGConfig.h"
#include "DGConsole.h"
#include "DGFont.h"
#include "DGLog.h"
#include "DGRender.h"


static DG_CONSOLE_STATE state;

void dg_console_init() {
	state.can_process = DG_NO;
	state.enabled = DG_NO;
	strcpy(state.command, "");
	strcpy(state.input, "");
	state.slide = config.display_height / 2;
}

void dg_console_release() {
	state.enabled = DG_NO;
}

void dg_console_delete() {
	if (state.enabled && strlen(state.input) > 0) {
		char aux[DG_MAX_LOGLEN];
		
		strcpy(aux, state.input);
		strcpy(state.input, "");
		strncat(state.input, aux, strlen(aux) - 1);
	}
}

void dg_console_enable() {
	state.enabled = DG_YES;
}

void dg_console_input(const char* key) {
	if (state.enabled) {
		strncat(state.input, key, 1);
	}
}

DG_BOOL dg_console_is_enabled() {
	return state.enabled;
}

DG_BOOL dg_console_output(char* buffer) {
	if (state.can_process) {
		strcpy(buffer, state.command);
		strcpy(state.command, "");
		state.can_process = DG_NO;
		
		return DG_YES;
	}	

	return DG_NO;
}

void dg_console_execute() {
	if (state.enabled) {
		strcpy(state.command, state.input);
		strcpy(state.input, "");
		state.can_process = DG_YES;
	}
}

void dg_console_toggle() {
	if (state.enabled)
		state.enabled = DG_NO;
	else {
		state.slide = config.display_height / 2;
		state.enabled = DG_YES;
	}
}

void dg_console_update() {
	if (state.enabled || (state.slide != (config.display_height / 2))) {
		char buffer[DG_MAX_LOGLEN];
		unsigned int index = 0, row = 2; // Row is two to avoid sticking to the margin and leave space for input
		
		dg_render_begin(DG_NO);
		dg_camera_set_ortho();
		while (dg_log_history(&index, buffer)) {
			short coords[] = { 0, config.display_height, 
				config.display_width, config.display_height,
				config.display_width, (config.display_height / 2) + state.slide,
				0, (config.display_height / 2) + state.slide };
			
			dg_render_with_color(0x88221111); // Put this into a "shade" type of color... oooor better use masks!! DG_COLOR_SHADE
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			dg_render_draw_quad(coords, DG_NO);
			 
			dg_render_with_color(dg_log_color(index));
			dg_font_print(10, (float)((config.display_height / 2) - state.slide - (15 * row)), buffer);
			
			row++;
		}
		
		dg_render_with_color(DG_COLOR_BRIGHTGREEN);
		dg_font_print(10, (float)((config.display_height / 2) - state.slide - 15), ">%s_", state.input);
		dg_camera_set_perspective();
		dg_render_end();
		
		if (state.enabled) {
			if ((state.slide - 10) > 0)
				state.slide -= 10;
			else
				state.slide = 0;
		}
		else {
			if ((state.slide + 10) < (config.display_height / 2))
				state.slide += 10;
			else
				state.slide = config.display_height / 2;
		}
	}
}
