/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGMemory.h"


static long mem_usage = 0;

void* DGAlloc(size_t size) {
	mem_usage += size;
	return malloc(size);
}

void DGFree(void* ptr) {
	mem_usage -= sizeof(ptr);
	free(ptr);
}

void* DGRealloc(void* ptr, size_t size) {
	mem_usage += (size - sizeof(ptr));
	return realloc(ptr, size);
}

long DGReportMem() {
	return mem_usage;
}

void* DGZeroMem(size_t num, size_t size) {
	mem_usage += (num * size);
	return calloc(num, size);
}
