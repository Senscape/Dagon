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
	DG_OBJECT* binded;
	DG_ARRAY* textures;
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

void dg_texture_manager_init() {
	//dg_log_trace(DG_MOD_TEXTURE, "Initializing texture manager...");
	
	Manager.textures = dg_array_new(0);
	Manager.active_textures = 0;
	Manager.binded = NULL;
}

void dg_texture_manager_release() {
	unsigned c;
	
	for (c = 0; c < dg_array_count(Manager.textures); c++) {
		dg_texture_release(dg_array_get_object(Manager.textures, c));
	}
	
	dg_array_release(Manager.textures);
	Manager.active_textures = 0;
	Manager.binded = NULL;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

void dg_texture_manager_bind(DG_OBJECT* texture) {
	dg_texture_bind(texture);
	Manager.binded = texture;
}

DG_OBJECT* dg_texture_manager_load(const char* name) {
	static unsigned position = 0;
	static char curr_name[DG_MAX_FILELEN];
	char tex_name[DG_MAX_FILELEN];
	char file_to_load[DG_MAX_FILELEN];
	DG_OBJECT* texture;
	
	if (strchr(name, '.')) { // We have a full filename, nothing else to be done
		strcpy(tex_name, name);
		strcpy(file_to_load, dg_config_get_path(DG_RES, name));
		position = 0; // Force position to 0 just in case
	}
	else {
		FILE* fh = NULL;
		
		// Calculate the position
		if (strcmp(curr_name, name) == 0) {
			position++;
		}
		else {
			// TODO: Really ugly, revise. Must avoid deleting active textures (load seq.)
			if (Manager.active_textures >= 18) {
				dg_texture_manager_release();
				dg_texture_manager_init();
			}
			
			position = 0;
			strcpy(curr_name, name);
		}
		
		// Generate the indexed filename
		_generate_filename(name, tex_name, position);
        
		// Test for pack
		strcpy(file_to_load, dg_config_get_path(DG_RES, _add_extension(name)));
		fh = fopen(file_to_load, "r");
		if (fh) { // Has a pack
			fclose(fh);
		}
		else { // No pack, so we use the indexed filename
			strcpy(file_to_load, dg_config_get_path(DG_RES, tex_name));
		}
	}
	
	if (!(texture = dg_array_get_object_by_name(Manager.textures, tex_name))) {
		texture = dg_texture_new(tex_name, config.tex_compression); // Not very safe
		dg_array_add_object(Manager.textures, texture);
		Manager.active_textures++;
	}
	
	if (!dg_texture_is_loaded(texture)) {
		dg_texture_load(texture, file_to_load, position);
    }
	
	return texture;
}

DG_OBJECT* dg_texture_manager_new(int width, int height, int depth) {
	DG_OBJECT* texture = dg_texture_new("", 1);
	dg_texture_clear(texture, width, height, depth);
	dg_array_add_object(Manager.textures, texture);
	Manager.active_textures++;
	
	return texture;
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

const char* _add_extension(const char* filename) {
	static char aux[DG_MAX_FILELEN];
	
	sprintf(aux, "%s.%s", filename, config.tex_extension);
	
	return aux;
}									

void _generate_filename(const char* filename, char* fileindexed, unsigned idx) {
	char aux[DG_MAX_FILELEN];
	
	strcpy(aux, filename);
	sprintf(fileindexed, "%s%0" in_between(DG_FILESEQZERO) "d.%s", strtok(aux, "."), idx + DG_FILESEQSTART, config.tex_extension);
}
