/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGArray.h"
#include "DGConfig.h"
#include "DGLog.h"
#include "DGTexture.h"
#include "DGTextureManager.h"


////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

struct {
	DGObject* binded;
	DGArray* textures;
	unsigned active_textures;
} Manager;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

const char* _add_extension(const char* filename);
void _generate_filename(const char* filename, char* fileindexed, unsigned idx);

////////////////////////////////////////////////////////////////////////////////
///// Standard Init / Release											   /////
////////////////////////////////////////////////////////////////////////////////

void DGTextureManagerInitialize() {
	//dg_log_trace(DG_MOD_TEXTURE, "Initializing texture manager...");
	
	Manager.textures = DGArrayNew(0);
	Manager.active_textures = 0;
	Manager.binded = NULL;
}

void DGTextureManagerTerminate() {
	unsigned c;
	
	for (c = 0; c < DGArrayCount(Manager.textures); c++) {
		DGTextureRelease(DGArrayGetObject(Manager.textures, c));
	}
	
	DGArrayRelease(Manager.textures);
	Manager.active_textures = 0;
	Manager.binded = NULL;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

void DGTextureManagerBind(DGObject* theTexture) {
	DGTextureBind(theTexture);
	Manager.binded = theTexture;
}

DGObject* DGTextureManagerLoad(const char* nameOfTexture) {
	static unsigned position = 0;
	static char curr_name[DG_MAX_FILELEN];
	char tex_name[DG_MAX_FILELEN];
	char file_to_load[DG_MAX_FILELEN];
	DGObject* texture;
	
	if (strchr(nameOfTexture, '.')) { // We have a full filename, nothing else to be done
		strcpy(tex_name, nameOfTexture);
		strcpy(file_to_load, DGConfigGetPath(DG_PATH_RES, nameOfTexture));
		position = 0; // Force position to 0 just in case
	}
	else {
		FILE* fh = NULL;
		
		// Calculate the position
		if (strcmp(curr_name, nameOfTexture) == 0) {
			position++;
		}
		else {
			// TODO: Really ugly, revise. Must avoid deleting active textures (load seq.)
			if (Manager.active_textures >= 18) {
				DGTextureManagerTerminate();
				DGTextureManagerInitialize();
			}
			
			position = 0;
			strcpy(curr_name, nameOfTexture);
		}
		
		// Generate the indexed filename
		_generate_filename(nameOfTexture, tex_name, position);
        
		// Test for pack
		strcpy(file_to_load, DGConfigGetPath(DG_PATH_RES, _add_extension(nameOfTexture)));
		fh = fopen(file_to_load, "r");
		if (fh) { // Has a pack
			fclose(fh);
		}
		else { // No pack, so we use the indexed filename
			strcpy(file_to_load, DGConfigGetPath(DG_PATH_RES, tex_name));
		}
	}
	
	if (!(texture = DGArrayGetObjectByName(Manager.textures, tex_name))) {
		texture = DGTextureNew(tex_name, DGConfig.tex_compression); // Not very safe
		DGArrayAddObject(Manager.textures, texture);
		Manager.active_textures++;
	}
	
	if (!DGTextureIsLoaded(texture)) {
		DGTextureLoad(texture, file_to_load, position);
    }
	
	return texture;
}

DGObject* DGTextureManagerNew(int width, int height, int depth) {
	DGObject* texture = DGTextureNew("", 1);
	DGTextureClear(texture, width, height, depth);
	DGArrayAddObject(Manager.textures, texture);
	Manager.active_textures++;
	
	return texture;
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

const char* _add_extension(const char* filename) {
	static char aux[DG_MAX_FILELEN];
	
	sprintf(aux, "%s.%s", filename, DGConfig.tex_extension);
	
	return aux;
}									

void _generate_filename(const char* filename, char* fileindexed, unsigned idx) {
	char aux[DG_MAX_FILELEN];
	
	strcpy(aux, filename);
	sprintf(fileindexed, "%s%0" in_between(DG_FILESEQZERO) "d.%s", strtok(aux, "."), idx + DG_FILESEQSTART, DGConfig.tex_extension);
}
