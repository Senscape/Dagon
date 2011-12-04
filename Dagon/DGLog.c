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

void dg_log_init() {
	FILE* result;
	
	history.size = 0;
	
	if (config.log) {
		// TODO: Optionally redirect all output (should go in config)
		result = freopen(DG_DEF_LOGFILE, "a", stdout);
		result = freopen(DG_DEF_LOGFILE, "a", stderr);
	}
}

void dg_log_release() {
	unsigned int i;
	
	for (i = 0; i < history.size; i++)
		dg_free(history.data[i]);
}

unsigned int dg_log_color(int index) {
	unsigned int i = index;

	if (i < history.size) {
		return history.data[i]->color;
	}
	else return DG_COLOR_WHITE;
}

DG_BOOL dg_log_history(unsigned int* index, char* buffer) {
	unsigned int my_index;
	
	my_index = *index;
	
	// TODO: the first line is not showing
	
	if (my_index <= 0)
		my_index = history.size;

	my_index--;
	if (my_index < history.size) {
		memcpy(buffer, history.data[my_index]->line, strlen(history.data[my_index]->line) + 1); // Last 1 is the NULL char

		*index = my_index;
		if (my_index > 0)
			return DG_YES;
		else
			return DG_NO;
	}
	
	return DG_NO;
}

void dg_log_command(int module, const char* string, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, string);
	vsprintf(data.line, string, ap);
	va_end(ap);
	
	data.color = DG_COLOR_BRIGHTMAGENTA;
	data.module = module;
	data.type = DG_LOG_TRACE;
	
	_log(&data);	
}

void dg_log_info(int module, const char* string, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, string);
	vsprintf(data.line, string, ap);
	va_end(ap);
	
	data.color = DG_COLOR_BRIGHTCYAN;
	data.module = module;
	data.type = DG_LOG_TRACE;
	
	_log(&data);	
}

void dg_log_trace(int module, const char* string, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, string);
	vsprintf(data.line, string, ap);
	va_end(ap);
	
	data.color = DG_COLOR_WHITE;
	data.module = module;
	data.type = DG_LOG_TRACE;
	
	_log(&data);	
}

void dg_log_error(int module, const char* string, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, string);
	vsprintf(data.line, string, ap);
	va_end(ap);

	data.color = DG_COLOR_BRIGHTRED;
	data.module = module;
	data.type = DG_LOG_ERROR;
	
	_log(&data);	
}

void dg_log_warning(int module, const char* string, ...) {
	DG_LOG_DATA data;
	va_list ap;
	
	va_start(ap, string);
	vsprintf(data.line, string, ap);
	va_end(ap);
	
	data.color = DG_COLOR_YELLOW;
	data.module = module;
	data.type = DG_LOG_WARNING;
	
	_log(&data);	
}

void _log(DG_LOG_DATA* data) {
	// TODO: Instead of opening and closing handles, we should flush the stream each time
	
	if (config.log) {
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

		file = fopen(dg_config_get_path(DG_USER, DG_DEF_LOGFILE), "a");
		if (file != NULL) {
			fprintf(file, "[%s] %s -> %s%s\n", timeFormatted, moduleName, logType, data->line);
			fclose(file);
		}
	}
	else {	
		printf("%s\n", data->line);
	}
	
	if (history.size < DG_MAX_LOGHIST) {
		history.data[history.size] = (DG_LOG_DATA*)dg_alloc(sizeof(DG_LOG_DATA));
		memcpy(history.data[history.size], data, sizeof(DG_LOG_DATA));
		history.size++;
	}
	else {
		int i;
		
		for (i = 0; i < (DG_MAX_LOGHIST - 1); i++)
			memcpy(history.data[i], history.data[i + 1], sizeof(DG_LOG_DATA));
		
		history.data[DG_MAX_LOGHIST - 1] = (DG_LOG_DATA*)dg_alloc(sizeof(DG_LOG_DATA));
		memcpy(history.data[DG_MAX_LOGHIST - 1], data, sizeof(DG_LOG_DATA));
	}
	
	fflush(stdout);
	fflush(stderr);
}
