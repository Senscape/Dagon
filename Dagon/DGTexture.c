/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGCommonGL.h"
#include "DGLog.h" // Not here
#include "DGMemory.h"
#include "DGTexture.h"

#include "stb_image.h"


////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
	char	name[DG_MAX_FILELEN];
	short	width;
	short	height;
	short	compression_level; // 0: None, 1: GL only, 2: GL & zlib
	short	num_textures;
} TEXMainHeader;

typedef struct {
	short	cube_position;
	short	depth;
	int		size;
	int		format;
} TEXSubHeader;

char TEXIdent[] = "KS_TEX"; // Tex files identifier

typedef struct {
	short compression_level;
	GLuint ident;
	GLint width;
	GLint height;
	GLint depth;
	GLubyte* bitmap;
	DG_BOOL loaded;
} DG_TEXTURE;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

DG_TEXTURE* _totexture(DG_OBJECT* texture);

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DG_OBJECT* dg_texture_new(const char* name, short compression_level) {
	DG_OBJECT* texture;
	DG_TEXTURE texture_data;
	
	glGenTextures(1, &texture_data.ident);  // Better put this on load
	texture_data.compression_level = compression_level;
	texture_data.loaded = DG_NO;
	
	// Store data into object and return
	texture = dg_object_new_named(DG_OBJ_TEXTURE, name, &texture_data, sizeof(texture_data));
	
	return texture;
}

void dg_texture_release(DG_OBJECT* texture) {
	DG_TEXTURE* texture_data = _totexture(texture);
	
	if (texture_data) {
		if (texture_data->loaded)
			glDeleteTextures(1, &texture_data->ident);
		
		dg_object_release(texture);
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

void dg_texture_append(DG_OBJECT* texture, const char* file) {
	DG_TEXTURE* texture_data = _totexture(texture);
	
	if (texture_data) {
		static int position = 0;
		FILE* fh;
		TEXMainHeader header;
		TEXSubHeader subheader;
		GLint internalformat, size;
		
		// TODO: Must check that the texture is RGB
		// NOTE: Let's try to support alpha channel for these textures
		if (!texture_data->loaded)
			return;

		glBindTexture(GL_TEXTURE_2D, texture_data->ident);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalformat);
		
		if (texture_data->compression_level) {
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size);
			texture_data->bitmap = (GLubyte*)dg_alloc(size * sizeof(GLubyte)); 
			glGetCompressedTexImage(GL_TEXTURE_2D, 0, texture_data->bitmap);
		}
		else {
			size = texture_data->width * texture_data->height * texture_data->depth;
			texture_data->bitmap = (GLubyte*)dg_alloc(size * sizeof(GLubyte)); 
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data->bitmap);
		}
		
		fh = fopen(file, "r+b");
		if (!fh) {
			fh = fopen(file, "wb");
			
			strcpy(header.name, texture->name); // This is the object name
			header.width = (short)texture_data->width;
			header.height = (short)texture_data->height;
			header.num_textures = 1;
			header.compression_level = texture_data->compression_level;
			
			fwrite(&TEXIdent, 1, 8, fh);
			fwrite(&header, 1, sizeof(header), fh);
			
			fclose(fh);
		}
		else {
			fseek(fh, 8, SEEK_SET); // Skip idenfitifer
			if (!fread(&header, 1, sizeof(header), fh))
				return;
			header.num_textures++;
			fseek(fh, 8, SEEK_SET); // Again
			fwrite(&header, 1, sizeof(header), fh);
			fclose(fh);
		}
		
		subheader.cube_position = position;
		subheader.depth = (short)texture_data->depth;
		subheader.size = (int)size;
		subheader.format = (int)internalformat;
		position++;
		
		fh = fopen(file, "ab");
		if (fh) {
			fwrite(&subheader, 1, sizeof(subheader), fh);
			fwrite(texture_data->bitmap, 1, sizeof(GLubyte) * size, fh);
			dg_free(texture_data->bitmap);
			fclose(fh);
		}
	}
}

void dg_texture_load(DG_OBJECT* texture, const char* file, unsigned index) {
	DG_TEXTURE* texture_data = _totexture(texture);
    
	if (texture_data) {	
		FILE* fh;
		char magic[10]; // Used to identity file types
		GLint format = 0, internalformat = 0;
		
		if (texture_data->loaded)
			return;
		
		fh = fopen(file, "rb");	
		
		if (fh != NULL) {
			if (fread(&magic, sizeof(magic), 1, fh) == 0) {
				// Couldn't read magic number
				dg_log_error(DG_MOD_TEXTURE, "%s", MSG210002, file);
			}
			
			if (memcmp(TEXIdent, &magic, 7) == 0) {
				short num_textures = 0;
				
				TEXMainHeader header;
				TEXSubHeader subheader;
				GLint compressed, size;
				
				fseek(fh, 8, SEEK_SET); // Skip identifier
				if (!fread(&header, 1, sizeof(header), fh))
					return;
				num_textures = header.num_textures;
				texture_data->width = (GLuint)header.width;
				texture_data->height = (GLuint)header.height;
				
				// Skip subheaders based on the index
				if (index) {
					unsigned int i;
					
					for (i = 0; i < index; i++) {
						if (!fread(&subheader, 1, sizeof(subheader), fh))
							return;
						fseek(fh, sizeof(char) * subheader.size, SEEK_CUR);
					}
				}
				
				if (!fread(&subheader, 1, sizeof(subheader), fh))
					return;
				
				texture_data->depth = (GLuint)subheader.depth;
				size = (GLint)subheader.size;
				internalformat = (GLint)subheader.format;
				
				texture_data->bitmap = (GLubyte*)dg_alloc(size * sizeof(GLubyte)); 
				if (!fread(texture_data->bitmap, 1, sizeof(GLubyte) * size, fh))
					return;
				
				glBindTexture(GL_TEXTURE_2D, texture_data->ident);
				
				if (header.compression_level) {
					glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalformat, texture_data->width, texture_data->height, 
										   0, size, texture_data->bitmap);
					glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
					
					if (compressed == GL_TRUE)
						texture_data->loaded = DG_YES;
					else
						dg_log_error(DG_MOD_TEXTURE, "%s", MSG210002, fh);
				}
				else {
					glTexImage2D(GL_TEXTURE_2D, 0, internalformat, texture_data->width, texture_data->height,
								 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data->bitmap);			// Only RGB is supported
					
					texture_data->loaded = DG_YES;
				}
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				
				
				dg_free(texture_data->bitmap);
			}
			else {
				int x, y, comp;
				
				fseek(fh, 0, SEEK_SET);
				texture_data->bitmap = (GLubyte*)stbi_load_from_file(fh, &x, &y, &comp, STBI_default);
				
				if (texture_data->bitmap) {
					texture_data->width = x;
					texture_data->height = y;
					texture_data->depth = comp;
					
					switch (comp) {
						case STBI_grey:
							format = GL_LUMINANCE;				
							if (texture_data->compression_level)							
								internalformat = GL_COMPRESSED_LUMINANCE;						
							else						
								internalformat = GL_LUMINANCE;
							break;
						case STBI_grey_alpha:
							format = GL_LUMINANCE_ALPHA;					
							if (texture_data->compression_level)
								internalformat = GL_COMPRESSED_LUMINANCE_ALPHA;							
							else						
								internalformat = GL_LUMINANCE_ALPHA;
							break;
						case STBI_rgb:
							format = GL_RGB;					
							if (texture_data->compression_level)					
								internalformat = GL_COMPRESSED_RGB;							
							else						
								internalformat = GL_RGB;
							break;
						case STBI_rgb_alpha:
							format = GL_RGBA;						
							if (texture_data->compression_level)						
								internalformat = GL_COMPRESSED_RGBA;						
							else						
								internalformat = GL_RGBA;
							break;
						default:
							dg_log_warning(DG_MOD_TEXTURE, "%s (%s) %d", MSG210003, file, comp);
							break;
					}
					
					glBindTexture(GL_TEXTURE_2D, texture_data->ident);
					
					glTexImage2D(GL_TEXTURE_2D, 0, internalformat, texture_data->width, texture_data->height,
								 0, format, GL_UNSIGNED_BYTE, texture_data->bitmap);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					
					dg_free(texture_data->bitmap);
					texture_data->loaded = DG_YES;
				}
				else {
					// Nothing loaded
					dg_log_error(DG_MOD_TEXTURE, "%s (%s) %s", MSG210001, file, stbi_failure_reason());
				}
			}
			
			fclose(fh);
		}
		else {
			// File not found
			dg_log_error(DG_MOD_TEXTURE, "%s %s", MSG210000, file);
		}
	}
}

void dg_texture_load_from_memory(DG_OBJECT* texture, const unsigned char* data) {
	DG_TEXTURE* texture_data = _totexture(texture);
	
	if (texture_data) {	
		int x, y, comp;
		GLint format = 0, internalformat = 0;
		
		if (texture_data->loaded)
			return;

		texture_data->bitmap = (GLubyte*)stbi_load_from_memory(data, 173487, &x, &y, &comp, STBI_default);
		
		if (texture_data->bitmap) {
			texture_data->width = x;
			texture_data->height = y;
			texture_data->depth = comp;
			
			switch (comp) {
				case STBI_grey:
					format = GL_LUMINANCE;				
					if (texture_data->compression_level)							
						internalformat = GL_COMPRESSED_LUMINANCE;						
					else						
						internalformat = GL_LUMINANCE;
					break;
				case STBI_grey_alpha:
					format = GL_LUMINANCE_ALPHA;					
					if (texture_data->compression_level)
						internalformat = GL_COMPRESSED_LUMINANCE_ALPHA;							
					else						
						internalformat = GL_LUMINANCE_ALPHA;
					break;
				case STBI_rgb:
					format = GL_RGB;					
					if (texture_data->compression_level)					
						internalformat = GL_COMPRESSED_RGB;							
					else						
						internalformat = GL_RGB;
					break;
				case STBI_rgb_alpha:
					format = GL_RGBA;						
					if (texture_data->compression_level)						
						internalformat = GL_COMPRESSED_RGBA;						
					else						
						internalformat = GL_RGBA;
					break;
				default:
					dg_log_warning(DG_MOD_TEXTURE, "%s %d", MSG210003, comp);
					break;
			}
			
			glBindTexture(GL_TEXTURE_2D, texture_data->ident);
			
			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, texture_data->width, texture_data->height,
						 0, format, GL_UNSIGNED_BYTE, texture_data->bitmap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			dg_free(texture_data->bitmap);
			texture_data->loaded = DG_YES;
		}
		else {
			// Nothing loaded
			dg_log_error(DG_MOD_TEXTURE, "%s %s", MSG210001, stbi_failure_reason());
		}
	}
}

void dg_texture_save(DG_OBJECT* texture, const char* file) {
	DG_TEXTURE* texture_data = _totexture(texture);
	
	if (texture_data) {	
		/*FILE* fh;
		TEXMainHeader header;
		TEXSubHeader subheader;
		GLint internalformat, size;
		
		// TODO: Must check that the texture is RGB
		// NOTE: Let's try to support alpha channel for these textures
		if (!texture_data->loaded)
			return;
		
		glBindTexture(GL_TEXTURE_2D, texture_data->ident);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalformat);
		
		if (texture_data->compression_level) {
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size);
			texture_data->bitmap = (GLubyte*)dg_alloc(size * sizeof(GLubyte)); 
			glGetCompressedTexImage(GL_TEXTURE_2D, 0, texture_data->bitmap);
			header.compression_level = 1;
		}
		else {
			size = texture_data->width * texture_data->height * texture_data->depth;
			texture_data->bitmap = (GLubyte*)dg_alloc(size * sizeof(GLubyte)); 
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data->bitmap);
			header.compression_level = 0;
		}
		
		strcpy(header.name, texture->name); // This is the object name
		header.width = (short)texture_data->width;
		header.height = (short)texture_data->height;
		header.num_textures = 0;
		
		subheader.cube_position = 0;
		subheader.depth = (short)texture_data->depth;
		subheader.size = (int)size;
		subheader.format = (int)internalformat;
		
		fh = fopen(file, "wb");
		fwrite(&TEXIdent, 1, 8, fh);
		fwrite(&header, 1, sizeof(header), fh);
		fwrite(&subheader, 1, sizeof(subheader), fh);
		fwrite(texture_data->bitmap, 1, sizeof(GLubyte) * size, fh);
		
		dg_free(texture_data->bitmap);
		fclose(fh);*/
		
		unsigned char cGarbage = 0, type,mode,aux;
		short int iGarbage = 0;
		int i;
		GLint size;
		FILE* fh;
		
		fh = fopen(file, "wb");
		if (fh == NULL)
			return;
		
		glBindTexture(GL_TEXTURE_2D, texture_data->ident);
		
		// We do this in case the texture wasn't loaded
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texture_data->width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texture_data->height);
		//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, &texture_data->depth);
		texture_data->depth = 24;
		
		mode = texture_data->depth / 8;
		if ((texture_data->depth == 24) || (texture_data->depth == 32))
			type = 2;
		else
			type = 3;
		
		fwrite(&cGarbage, sizeof(unsigned char), 1, fh);
		fwrite(&cGarbage, sizeof(unsigned char), 1, fh);
		
		fwrite(&type, sizeof(unsigned char), 1, fh);
		
		fwrite(&iGarbage, sizeof(short int), 1, fh);
		fwrite(&iGarbage, sizeof(short int), 1, fh);
		fwrite(&cGarbage, sizeof(unsigned char), 1, fh);
		fwrite(&iGarbage, sizeof(short int), 1, fh);
		fwrite(&iGarbage, sizeof(short int), 1, fh);
		
		fwrite(&texture_data->width, sizeof(short int), 1, fh);
		fwrite(&texture_data->height, sizeof(short int), 1, fh);
		fwrite(&texture_data->depth, sizeof(unsigned char), 1, fh);
		
		fwrite(&cGarbage, sizeof(unsigned char), 1, fh);
		
		texture_data->depth = mode;
		
		size = texture_data->width * texture_data->height * texture_data->depth;
		texture_data->bitmap = (GLubyte*)dg_alloc(size * sizeof(GLubyte)); 
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data->bitmap);
		
		if (mode >= 3)
			for (i = 0; i < texture_data->width * texture_data->height * mode ; i+= mode) {
				aux = texture_data->bitmap[i];
				texture_data->bitmap[i] = texture_data->bitmap[i + 2];
				texture_data->bitmap[i + 2] = aux;
			}
		
		fwrite(texture_data->bitmap, 1, sizeof(GLubyte) * size, fh);
		fclose(fh);

		dg_free(texture_data->bitmap);
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Checks											   /////
////////////////////////////////////////////////////////////////////////////////

unsigned dg_texture_count(const char* file) {
	FILE* fh;
	char magic[10]; // Used to identity file types

	fh = fopen(file, "rb");	
	
	if (fh != NULL) {
		if (fread(&magic, sizeof(magic), 1, fh) == 0) {
			// Couldn't read magic number
			dg_log_error(DG_MOD_TEXTURE, "%s", MSG210002, file);
		}
		
		if (memcmp(TEXIdent, &magic, 7) == 0) {
			TEXMainHeader header;
			
			fseek(fh, 8, SEEK_SET); // Skip identifier
			if (!fread(&header, 1, sizeof(header), fh))
				return -2; // Invalid size
			
			fclose(fh);
			
			return header.num_textures;
		}
	}
	
	return -1; // Undefined error
}

unsigned dg_texture_height(DG_OBJECT* texture) {
	DG_TEXTURE* texture_data = _totexture(texture);
	
	if (texture_data) {	
		return texture_data->height;
	}
	
	return 0;
}

DG_BOOL dg_texture_is_loaded(DG_OBJECT* texture) {
	DG_TEXTURE* texture_data = _totexture(texture);
	
	if (texture_data) {	
		return texture_data->loaded;
	}
	
	return DG_NO;
}

unsigned dg_texture_width(DG_OBJECT* texture) {
	DG_TEXTURE* texture_data = _totexture(texture);
	
	if (texture_data) {	
		return texture_data->width;
	}
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void dg_texture_bind(DG_OBJECT* texture) {
	DG_TEXTURE* texture_data = _totexture(texture);
	
	if (texture_data) {		
		glBindTexture(GL_TEXTURE_2D, texture_data->ident);
	}
}

void dg_texture_clear(DG_OBJECT* texture, int width, int height, int depth) {
	DG_TEXTURE* texture_data = _totexture(texture);
	
	if (texture_data) {	
		if (texture_data->loaded) {
			glDeleteTextures(1, &texture_data->ident);
			texture_data->loaded = DG_NO;
		}
		else { // TODO: This is flacky, revise
			unsigned int* data;
			int comp;
			
			if (!width) width = DG_DEF_TEXSIZE;
			if (!height) height = DG_DEF_TEXSIZE;
			if (!depth) comp = 3;
			else comp = depth / 8;
			
			data = (unsigned int*)dg_zeromem(width * height * comp, sizeof(GLuint));
			
			// WARNING: This code doesn't handle components properly
			// Generating textures twice?
			//glGenTextures(1, &texture_data->ident);
			glBindTexture(GL_TEXTURE_2D, texture_data->ident);
			glTexImage2D(GL_TEXTURE_2D, 0, comp, width, height,
						 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			//texture_data->loaded = DG_YES;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

DG_TEXTURE* _totexture(DG_OBJECT* texture) {
	if (dg_object_check(texture, DG_OBJ_TEXTURE)) {
		DG_TEXTURE* texture_data = (DG_TEXTURE*)dg_object_data(texture);
		return texture_data;
	}
	else {
		// raise error
		return NULL;
	}
}
