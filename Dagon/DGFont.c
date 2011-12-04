/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGCamera.h" // Ugly
#include "DGCommonGL.h"
#include "DGConfig.h"
#include "DGFont.h"
#include "DGLog.h"
#include "DGMemory.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define sizeof_var( var ) ((size_t)(&(var)+1)-(size_t)(&(var)))


typedef struct {
	float x;
	float y;
	int width;
	int rows;
	int left;
	int top;
	int advance;
} DG_GLYPH;

typedef struct {
	float h;
	GLuint* textures;
	DG_GLYPH glyph[128];
} DG_FONT;

DG_FONT font_data;
DG_FONT* ft_font; // Using this until we create a proper array

int next_p2 (int a);

int next_p2 (int a) {
	int rval = 1;
	while (rval < a) rval <<= 1;
	return rval;
}

void dg_font_init(const char *file, unsigned int height) {
	unsigned char ch;	
	FT_Library library;
	FT_Face face;
	/*FT_Byte* buffer;
	long size;*/
	
	dg_log_trace(DG_MOD_FONT, "Initializing font manager...");
	dg_log_info(DG_MOD_FONT, "Freetype version: %d.%d.%d", FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH);
	
	ft_font = &font_data; // Temp
	ft_font->textures = (GLuint*)dg_alloc(128 * sizeof(GLuint));
	ft_font->h = (float)height;

	/*fh = fopen(dg_config_get_path(DG_RES, file), "rb");
	 if (fh) {
	 fseek(fh, 0L, SEEK_END);
	 size = ftell(fh);
	 fseek(fh, 0L, SEEK_SET);
	 buffer = (FT_Byte*)dg_alloc(size * sizeof(FT_Byte));
	 fread(buffer, sizeof(FT_Byte), size, fh);
	 fclose(fh);
	 }
	 else return;*/
	
	if (FT_Init_FreeType(&library)) 
		dg_log_error(DG_MOD_FONT, "Failed to initialize font library!");

	// WARNING: No way of determining that 49052.. Careful!
	if (FT_New_Memory_Face(library, _binary_def_font_start, 49052, 0, &face)) 
		dg_log_error(DG_MOD_FONT, "Requested font is corrupt: %s", file);
	
	FT_Set_Char_Size(face, height << 6, height << 6, 96, 96);

	glGenTextures(128, ft_font->textures);
	
	for (ch = 0; ch < 128; ch++) {
		int i, j;
		float x, y;
		int width, height;
		FT_Glyph glyph;
		FT_Bitmap bitmap;
		FT_BitmapGlyph bitmap_glyph;
		GLubyte* expanded_data;
		
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
			dg_log_error(DG_MOD_FONT, "Error loading glyph: %c", ch);
		
		if (FT_Get_Glyph(face->glyph, &glyph))
			dg_log_error(DG_MOD_FONT, "Error getting glyph: %c", ch);
		
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		bitmap_glyph = (FT_BitmapGlyph)glyph;
		
		bitmap = bitmap_glyph->bitmap;
		
		width = next_p2(bitmap.width);
		height = next_p2(bitmap.rows);
		
		expanded_data = (GLubyte*)dg_alloc(2 * width * height); // Is there a sizeof() missing here?
		
		for (j = 0; j < height; j++) {
			for (i = 0; i < width; i++) {
				expanded_data[2 * (i + j *width)] = expanded_data[2 * (i + j * width) + 1] = 
				(i >= bitmap.width || j >= bitmap.rows) ?
				0 : bitmap.buffer[i + bitmap.width * j];
			}
		}
		
		x = (float)bitmap.width / (float)width;
		y = (float)bitmap.rows / (float)height;
		
		ft_font->glyph[ch].x = x; 
		ft_font->glyph[ch].y = y;
		ft_font->glyph[ch].width = bitmap.width;
		ft_font->glyph[ch].rows = bitmap.rows;
		ft_font->glyph[ch].left = bitmap_glyph->left;
		ft_font->glyph[ch].top = bitmap_glyph->top;
		ft_font->glyph[ch].advance = face->glyph->advance.x;
		
		glBindTexture(GL_TEXTURE_2D, ft_font->textures[ch]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
					 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);
		
		dg_free(expanded_data);
	}
	
	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void dg_font_clean() {
	glDeleteTextures(128, ft_font->textures);
	dg_free(ft_font->textures);
}

void dg_font_print(float x, float y, const char* text, ...) {
	// TODO: Maybe there should be a dg_render_print() function, and use this one...
	
	unsigned int i, j;
	float h;
	
	// We should keep this entire buffer separated from this function
	char aux[DG_MAX_FEEDBACK];
	char line[DG_MAX_FEEDBACK];
	char lines[DG_MAX_LOGHIST][DG_MAX_FEEDBACK]; // Max rows of text
	unsigned int size = 0;
	const char* c;
	
	va_list	ap;
	
	h = ft_font->h / .63f;
	
	if (text == NULL)
		*line=0;
	else {
		va_start(ap, text);
	    vsprintf(line, text, ap);
		va_end(ap);
	}
	
	strcpy(aux, "");
	for (c = line; *c; c++) {
		if (*c == '\n') {
			strcpy(lines[size], aux);
			strcpy(aux, "");
			size++;
		}
		else {
			strncat(aux, c, 1);
		}
	}
	
	// This is to copy the last line
	strcpy(lines[size], aux);
	size++;
	
	dg_camera_set_ortho();
	
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//y = (config.display_height - y) - 32; // Not sure why we have to substract 32 on Mac...
	y = (config.display_height - y);
	
	for (i = 0; i < size; i++) {
		c = lines[i];
			
		glPushMatrix();
		glTranslatef(x, y - h * i, 0);
		
		// Uncomment to calculate length
		//glRasterPos2f(0, 0);
			
		for (j = 0; j < strlen(lines[i]); j++) {
			int ch = *c;
			
			GLfloat texCoords[] = {	0, 0,
				0, ft_font->glyph[ch].y,
				ft_font->glyph[ch].x, ft_font->glyph[ch].y,
				ft_font->glyph[ch].x, 0 };
			
			GLshort coords[] = { 0, ft_font->glyph[ch].rows,
				0, 0,
				ft_font->glyph[ch].width, 0,
				ft_font->glyph[ch].width, ft_font->glyph[ch].rows };
			
			glBindTexture(GL_TEXTURE_2D, ft_font->textures[ch]);
			
			glTranslatef((GLfloat)ft_font->glyph[ch].left, 0, 0);
			
			glPushMatrix();
			glTranslatef(0, (GLfloat)(ft_font->glyph[ch].top - ft_font->glyph[ch].rows), 0);
			
			glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
			glVertexPointer(2, GL_SHORT, 0, coords);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			
			glPopMatrix();
			glTranslatef((GLfloat)(ft_font->glyph[ch].advance >> 6), 0, 0);
			
			c++;
		}
		
		//float rpos[4];
		//glGetFloatv(GL_CURRENT_RASTER_POSITION, rpos);
		//float len = x - rpos[0];
		
		glPopMatrix();
	}
	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glPopAttrib();
	
	dg_camera_set_perspective();
}
