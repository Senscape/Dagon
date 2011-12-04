/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGLog.h"
#include "DGState.h"


struct dg_state_info {
	int current;
	int last;
} state;

void dg_state_init() {
	dg_log_trace(DG_MOD_STATE, "Initializing state machine...");
	
	state.current = DG_ST_NODE;
	state.last = state.current;
}

unsigned dg_state_current() {
	return state.current;
}

unsigned dg_state_last() {
	return state.last;
}

void dg_state_return() {
	unsigned auxState = state.current;
	
	state.current = state.last;
	state.last = auxState;
}

void dg_state_set(unsigned new_state) {
	state.last = state.current;
	state.current = new_state;
}
