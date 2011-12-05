/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_LOG_H
#define	DG_LOG_H

#include "DGCommon.h"


enum DG_LOG_MODULES {
	DG_MOD_AUDIO,
	DG_MOD_CAMERA,
	DG_MOD_CONFIG,
	DG_MOD_CONTROL,
	DG_MOD_CORE,
	DG_MOD_EFFECTS,
	DG_MOD_FONT,
	DG_MOD_RENDER,
	DG_MOD_SCRIPT,
	DG_MOD_STATE,
	DG_MOD_SYSTEM,
	DG_MOD_TEXTURE,
	DG_MOD_VIDEO,
	DG_MOD_NONE
};

enum DG_LOG_TYPES {
	DG_LOG_TRACE,
	DG_LOG_ERROR,
	DG_LOG_WARNING
};

void    DGLogInitialize(void);
void    DGLogTerminate(void);
void    DGLogCommand(int forModule, const char* theString, ...);
void    DGLogInfo(int forModule, const char* theString, ...);
void    DGLogError(int forModule, const char* theString, ...);
void    DGLogTrace(int forModule, const char* theString, ...);
void    DGLogWarning(int forModule, const char* theString, ...);

unsigned int	DGLogColor(int atIndex); // Retrieves the color for the console feature
DGBool			DGLogHistory(unsigned int* pointerToIndex, char* pointerToBuffer); // Cycles through the current buffer

#endif // DG_LOG_H
