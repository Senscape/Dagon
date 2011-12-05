/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGConfig.h"
#include "DGLog.h"
#include "DGMemory.h"


typedef struct {
	char line[DG_MAX_LOGLEN];
	unsigned int color;
	unsigned int module;
	unsigned int type;
} DG_LOG_DATA;

struct {
	DG_LOG_DATA* data[DG_MAX_LOGHIST];
	unsigned int size;
} history;

void _log(DG_LOG_DATA* data);

void DGLogInitialize() {
	FILE* result;
	
	history.size = 0;
	
	if (DGConfig.log) {
		// TODO: Optionally redirect all output (should go in config)
		result = freopen(DG_DEF_LOGFILE, "a", stdout);
		result = freopen(DG_DEF_LOGFILE, "a", stderr);
	}
}

void DGLogTerminate() {
	unsigned int i;
	
	for (i = 0; i < history.size; i++)
		DGFree(history.data[i]);
}

unsigned int DGLogColor(int atIndex) {
	unsigned int i = atIndex;

	if (i < history.size) {
		return history.data[i]->color;
	}
	else return DG_COLOR_WHITE;
}

DGBool DGLogHistory(unsigned int* pointerToIndex, char* pointerToBuffer) {
	unsigned int my_index;
	
	my_index = *pointerToIndex;
	
	// TODO: the first line is not showing
	
	if (my_index <= 0)
		my_index = history.size;

	my_index--;
	if (my_index < history.size) {
		memcpy(pointerToBuffer, history.data[my_index]->line, strlen(history.data[my_index]->line) + 1); // Last 1 is the NULL char

		*pointerToIndex = my_index;
		if (my_index > 0)
			return DG_YES;
		else
			return DG_NO;
	}
	
	return DG_NO;
}

void DGLogCommand(int forModule, const char* theString, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);
	
	data.color = DG_COLOR_BRIGHTMAGENTA;
	data.module = forModule;
	data.type = DG_LOG_TRACE;
	
	_log(&data);	
}

void DGLogInfo(int forModule, const char* theString, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);
	
	data.color = DG_COLOR_BRIGHTCYAN;
	data.module = forModule;
	data.type = DG_LOG_TRACE;
	
	_log(&data);	
}

void DGLogTrace(int forModule, const char* theString, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);
	
	data.color = DG_COLOR_WHITE;
	data.module = forModule;
	data.type = DG_LOG_TRACE;
	
	_log(&data);	
}

void DGLogError(int forModule, const char* theString, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);

	data.color = DG_COLOR_BRIGHTRED;
	data.module = forModule;
	data.type = DG_LOG_ERROR;
	
	_log(&data);	
}

void DGLogWarning(int forModule, const char* theString, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);
	
	data.color = DG_COLOR_YELLOW;
	data.module = forModule;
	data.type = DG_LOG_WARNING;
	
	_log(&data);	
}

void _log(DG_LOG_DATA* data) {
	// TODO: Instead of opening and closing handles, we should flush the stream each time
	
	if (DGConfig.log) {
		FILE*       file;
		time_t      rawtime;
		char*		p;
		char        logType[80], moduleName[80], timeFormatted[50];
		
		time(&rawtime);
		strcpy(timeFormatted, (char*)ctime(&rawtime));
		p = strrchr(timeFormatted, '\n');
		*p = '\0';
		
		switch (data->type) {
			case DG_LOG_TRACE:
				strcpy(logType, "");
				break;
			case DG_LOG_ERROR:
				strcpy(logType, "ERROR: ");
				break;		
			case DG_LOG_WARNING:
				strcpy(logType, "WARNING: ");
				break;
		}
			
		switch (data->module) {
			case DG_MOD_AUDIO:
				strcpy(moduleName, "        AUDIO");
				break;
			case DG_MOD_CAMERA:
				strcpy(moduleName, "       CAMERA");
				break;
			case DG_MOD_CONFIG:
				strcpy(moduleName, "CONFIGURATION");
				break;					
			case DG_MOD_CONTROL:
				strcpy(moduleName, "   CONTROLLER");
				break;			
			case DG_MOD_CORE:
				strcpy(moduleName, "         CORE");
				break;
			case DG_MOD_EFFECTS:
				strcpy(moduleName, "      EFFECTS");
				break;
			case DG_MOD_FONT:
				strcpy(moduleName, "         FONT");
				break;	
			case DG_MOD_RENDER:
				strcpy(moduleName, "       RENDER");
				break;
			case DG_MOD_SCRIPT:
				strcpy(moduleName, "       SCRIPT");
				break;
			case DG_MOD_STATE:
				strcpy(moduleName, "        STATE");
				break;
			case DG_MOD_SYSTEM:
				strcpy(moduleName, "       SYSTEM");
				break;				
			case DG_MOD_TEXTURE:
				strcpy(moduleName, "      TEXTURE");
				break;
			case DG_MOD_VIDEO:
				strcpy(moduleName, "        VIDEO");
				break;
			case DG_MOD_NONE:
			default:
				strcpy(moduleName, "      GENERAL");
				break;
		}

		file = fopen(DGConfigGetPath(DG_PATH_USER, DG_DEF_LOGFILE), "a");
		if (file != NULL) {
			fprintf(file, "[%s] %s -> %s%s\n", timeFormatted, moduleName, logType, data->line);
			fclose(file);
		}
	}
	else {	
		printf("%s\n", data->line);
	}
	
	if (history.size < DG_MAX_LOGHIST) {
		history.data[history.size] = (DG_LOG_DATA*)DGAlloc(sizeof(DG_LOG_DATA));
		memcpy(history.data[history.size], data, sizeof(DG_LOG_DATA));
		history.size++;
	}
	else {
		int i;
		
		for (i = 0; i < (DG_MAX_LOGHIST - 1); i++)
			memcpy(history.data[i], history.data[i + 1], sizeof(DG_LOG_DATA));
		
		history.data[DG_MAX_LOGHIST - 1] = (DG_LOG_DATA*)DGAlloc(sizeof(DG_LOG_DATA));
		memcpy(history.data[DG_MAX_LOGHIST - 1], data, sizeof(DG_LOG_DATA));
	}
	
	fflush(stdout);
	fflush(stderr);
}
