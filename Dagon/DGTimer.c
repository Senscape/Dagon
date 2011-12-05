/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGTimer.h"


typedef struct {
    unsigned id;
	unsigned trigger;
    unsigned elapsed;
    DGBool enabled;
    int ref;
} DG_TIMER;

static unsigned idx = 0;
static DG_TIMER timers[100];

unsigned dg_timer_start(int secs, int ref) {
    timers[idx].id = idx;
    timers[idx].elapsed = 0;
    timers[idx].trigger = secs;    
    timers[idx].enabled = DG_YES;
    timers[idx].ref = ref;
    
    idx++;
    
    return idx-1;
}

void dg_timer_pause(unsigned id) {
}

void dg_timer_stop(unsigned id) {
}

int dg_timer_check() {
    int             i;
	clock_t         currentTime = clock();
	static clock_t  lastTime = 0;
	double          duration = (double)(currentTime - lastTime) / CLOCKS_PER_SEC;
    
    if (duration >= 1) {
        lastTime = currentTime;
        for (i = 0; i < idx; i++) {
            if (timers[i].enabled) {
                timers[i].elapsed++;
                if (timers[i].elapsed >= timers[i].trigger) {
                    timers[i].elapsed = 0;
                    timers[i].enabled = DG_NO;
                    
                    return timers[i].ref;
                }
            }
        }        
    }
    
    return 0;
}
