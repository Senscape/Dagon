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

void DGStateInitialize() {
	DGLogTrace(DG_MOD_STATE, "Initializing state machine...");
	
	state.current = DG_STATE_NODE;
	state.last = state.current;
}

unsigned DGStateCurrent() {
	return state.current;
}

unsigned DGStateLast() {
	return state.last;
}

void DGStateReturn() {
	unsigned auxState = state.current;
	
	state.current = state.last;
	state.last = auxState;
}

void DGStateSet(unsigned theState) {
	state.last = state.current;
	state.current = theState;
}
