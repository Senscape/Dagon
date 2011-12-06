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


static DGConsoleState state;

void DGConsoleInitialize() {
	state.can_process = DG_NO;
	state.enabled = DG_NO;
	strcpy(state.command, "");
	strcpy(state.input, "");
	state.slide = DGConfig.display_height / 2;
}

void DGConsoleTerminate() {
	state.enabled = DG_NO;
}

void DGConsoleDeleteChar() {
	if (state.enabled && strlen(state.input) > 0) {
		char aux[DG_MAX_LOGLEN];
		
		strcpy(aux, state.input);
		strcpy(state.input, "");
		strncat(state.input, aux, strlen(aux) - 1);
	}
}

void DGConsoleEnable() {
	state.enabled = DG_YES;
}

void DGConsoleInputChar(const char* aKey) {
	if (state.enabled) {
		strncat(state.input, aKey, 1);
	}
}

DGBool DGConsoleIsEnabled() {
	return state.enabled;
}

DGBool DGConsoleOutput(char* pointerToBuffer) {
	if (state.can_process) {
		strcpy(pointerToBuffer, state.command);
		strcpy(state.command, "");
		state.can_process = DG_NO;
		
		return DG_YES;
	}	

	return DG_NO;
}

void DGConsoleExecute() {
	if (state.enabled) {
		strcpy(state.command, state.input);
		strcpy(state.input, "");
		state.can_process = DG_YES;
	}
}

void DGConsoleToggleState() {
	if (state.enabled)
		state.enabled = DG_NO;
	else {
		state.slide = DGConfig.display_height / 2;
		state.enabled = DG_YES;
	}
}

void DGConsoleUpdate() {
	if (state.enabled || (state.slide != (DGConfig.display_height / 2))) {
		char buffer[DG_MAX_LOGLEN];
		unsigned int index = 0, row = 2; // Row is two to avoid sticking to the margin and leave space for input
		
		DGRenderBegin(DG_NO);
		DGCameraSetOrthoView();
		while (DGLogHistory(&index, buffer)) {
			short coords[] = { 0, DGConfig.display_height, 
				DGConfig.display_width, DGConfig.display_height,
				DGConfig.display_width, (DGConfig.display_height / 2) + state.slide,
				0, (DGConfig.display_height / 2) + state.slide };
			
			DGRenderSetColor(0x88221111); // Put this into a "shade" type of color... oooor better use masks!! DG_COLOR_SHADE
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			DGRenderDrawOverlay(coords, DG_NO);
			 
			DGRenderSetColor(DGLogColor(index));
			DGFontPrint(10, (float)((DGConfig.display_height / 2) - state.slide - (15 * row)), buffer);
            
			row++;
		}
		
		DGRenderSetColor(DG_COLOR_BRIGHTGREEN);
		DGFontPrint(10, (float)((DGConfig.display_height / 2) - state.slide - 15), ">%s_", state.input);
		DGCameraSetPerspectiveView();
		DGRenderEnd();
		
		if (state.enabled) {
			if ((state.slide - 10) > 0)
				state.slide -= 10;
			else
				state.slide = 0;
		}
		else {
			if ((state.slide + 10) < (DGConfig.display_height / 2))
				state.slide += 10;
			else
				state.slide = DGConfig.display_height / 2;
		}
	}
}
