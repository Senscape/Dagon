////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "DGLanguage.h"
#include "DGTexture.h"
#include "stb_image.h"

////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////

//char TEXIdent[] = "DG_TEX"; // Tex files identifier
char TEXIdent[] = "KS_TEX"; // Tex files identifier

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGTexture::DGTexture() {
    config = &DGConfig::getInstance();
    log = &DGLog::getInstance();

    _compressionLevel = config->texCompression;
    _hasResource = false;
	_isLoaded = false;
    
    _usageCount = 0;
}

DGTexture::DGTexture(int width, int height, int depth) {
    int comp;
    
    config = &DGConfig::getInstance();
    log = &DGLog::getInstance();
    
    if (!width) width = DGDefTexSize;
    if (!height) height = DGDefTexSize;
    if (!depth) comp = 3;
    else comp = depth / 8;
    
    _bitmap = (GLubyte*)calloc(width * height * comp, sizeof(GLubyte));
    
    glGenTextures(1, &_ident);
    glBindTexture(GL_TEXTURE_2D, _ident);
    glTexImage2D(GL_TEXTURE_2D, 0, comp, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, _bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    free(_bitmap);
    
    _compressionLevel = config->texCompression;
    
    // The texture doesn't require a resource, so we make it clear
    _hasResource = true;
    _isLoaded = true;
    
    // Since the texture will be loaded only once, we note this
    _usageCount = 1;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGTexture::~DGTexture() {
    this->unload();
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGTexture::hasResource() {
    return _hasResource;
}

bool DGTexture::isLoaded() {
    return _isLoaded;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

int DGTexture::depth() {
    return _depth;
}

int DGTexture::indexInBundle() {
    return _indexInBundle;
}

int DGTexture::height() {
    return _height;
}

const char* DGTexture::resource() {
    return _resource;
}


unsigned int DGTexture::usageCount() {
    return _usageCount;
}

int DGTexture::width() {
    return _width;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGTexture::increaseUsageCount() {
    // We only keep trace of the usage count if the
    // texture is loaded
    if (_isLoaded)
        _usageCount++;
}

void DGTexture::setIndexInBundle(int index) {
    _indexInBundle = index;
}

void DGTexture::setResource(const char* fromFileName) {
    strncpy(_resource, fromFileName, DGMaxFileLength);
    _hasResource = true;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGTexture::bind() {
    if (_isLoaded)
        glBindTexture(GL_TEXTURE_2D, _ident);
}

void DGTexture::load() {
    FILE* fh;
    char magic[10]; // Used to identity file types
    GLint format = 0, internalFormat = 0;
    
    if (_isLoaded)
        return;
    
    if (!_hasResource) {
        log->error(DGModTexture, "%s: %s", DGMsg210004, this->name());
        
        return;
    }
    
    fh = fopen(_resource, "rb");	
    
    if (fh != NULL) {
        if (fread(&magic, sizeof(magic), 1, fh) == 0) {
            // Couldn't read magic number
            log->error(DGModTexture, "%s: %s", DGMsg210002, _resource);
        }
        
        if (memcmp(TEXIdent, &magic, 7) == 0) {
            short numTextures = 0;
            
            TEXMainHeader header;
            TEXSubHeader subheader;
            GLint compressed, size;
            
            fseek(fh, 8, SEEK_SET); // Skip identifier
            if (!fread(&header, 1, sizeof(header), fh))
                return;
            numTextures = header.numTextures;
            _width = (GLuint)header.width;
            _height = (GLuint)header.height;
            
            // Skip subheaders based on the index
            if (_indexInBundle) {
                unsigned int i;
                
                for (i = 0; i < _indexInBundle; i++) {
                    if (!fread(&subheader, 1, sizeof(subheader), fh))
                        return;
                    fseek(fh, sizeof(char) * subheader.size, SEEK_CUR);
                }
            }
            
            if (!fread(&subheader, 1, sizeof(subheader), fh))
                return;
            
            _depth = (GLuint)subheader.depth;
            size = (GLint)subheader.size;
            internalFormat = (GLint)subheader.format;
            
            _bitmap = (GLubyte*)malloc(size * sizeof(GLubyte)); 
            if (!fread(_bitmap, 1, sizeof(GLubyte) * size, fh))
                return;
            
            glGenTextures(1, &_ident);
            glBindTexture(GL_TEXTURE_2D, _ident);
            
            if (header.compressionLevel) {
                glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 
                                       0, size, _bitmap);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
                
                if (compressed == GL_TRUE)
                    _isLoaded = true;
                else
                    log->error(DGModTexture, "%s: %s", DGMsg210002, fh);
            }
            else {
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _bitmap); // Only RGB is supported
                _isLoaded = true;
            }
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            free(_bitmap);
        }
        else {
            int x, y, comp;
            
            fseek(fh, 0, SEEK_SET);
            _bitmap = (GLubyte*)stbi_load_from_file(fh, &x, &y, &comp, STBI_default);
            
            if (_bitmap) {
                _width = x;
                _height = y;
                _depth = comp;
                
                switch (comp) {
                    case STBI_grey:
                        format = GL_LUMINANCE;				
                        if (_compressionLevel)							
                            internalFormat = GL_COMPRESSED_LUMINANCE;						
                        else						
                            internalFormat = GL_LUMINANCE;
                        break;
                    case STBI_grey_alpha:
                        format = GL_LUMINANCE_ALPHA;					
                        if (_compressionLevel)
                            internalFormat = GL_COMPRESSED_LUMINANCE_ALPHA;							
                        else						
                            internalFormat = GL_LUMINANCE_ALPHA;
                        break;
                    case STBI_rgb:
                        format = GL_RGB;					
                        if (_compressionLevel)					
                            internalFormat = GL_COMPRESSED_RGB;							
                        else						
                            internalFormat = GL_RGB;
                        break;
                    case STBI_rgb_alpha:
                        format = GL_RGBA;						
                        if (_compressionLevel)						
                            internalFormat = GL_COMPRESSED_RGBA;						
                        else						
                            internalFormat = GL_RGBA;
                        break;
                    default:
                        log->warning(DGModTexture, "%s: (%s) %d", DGMsg210003, _resource, comp);
                        break;
                }
                
                glGenTextures(1, &_ident);
                glBindTexture(GL_TEXTURE_2D, _ident);
                
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height,
                             0, format, GL_UNSIGNED_BYTE, _bitmap);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                
                free(_bitmap);
                
                _isLoaded = true;
            }
            else {
                // Nothing loaded
                log->error(DGModTexture, "%s: (%s) %s", DGMsg210001, _resource, stbi_failure_reason());
            }
        }
        
        fclose(fh);
    }
    else {
        // File not found
        log->error(DGModTexture, "%s: %s", DGMsg210000, _resource);
    }    
}

void DGTexture::loadFromMemory(const unsigned char* dataToLoad) {
    int x, y, comp;
    GLint format = 0, internalFormat = 0;
    
    if (_isLoaded)
        return;
    
    _bitmap = (GLubyte*)stbi_load_from_memory(dataToLoad, 173487, &x, &y, &comp, STBI_default);
    
    if (_bitmap) {
        _width = x;
        _height = y;
        _depth = comp;
        
        switch (comp) {
            case STBI_grey:
                format = GL_LUMINANCE;				
                if (_compressionLevel)							
                    internalFormat = GL_COMPRESSED_LUMINANCE;						
                else						
                    internalFormat = GL_LUMINANCE;
                break;
            case STBI_grey_alpha:
                format = GL_LUMINANCE_ALPHA;					
                if (_compressionLevel)
                    internalFormat = GL_COMPRESSED_LUMINANCE_ALPHA;							
                else						
                    internalFormat = GL_LUMINANCE_ALPHA;
                break;
            case STBI_rgb:
                format = GL_RGB;					
                if (_compressionLevel)					
                    internalFormat = GL_COMPRESSED_RGB;							
                else						
                    internalFormat = GL_RGB;
                break;
            case STBI_rgb_alpha:
                format = GL_RGBA;						
                if (_compressionLevel)						
                    internalFormat = GL_COMPRESSED_RGBA;						
                else						
                    internalFormat = GL_RGBA;
                break;
            default:
                log->warning(DGModTexture, "%s: %d", DGMsg210003, comp);
                break;
        }
        
        glGenTextures(1, &_ident);
        glBindTexture(GL_TEXTURE_2D, _ident);
        
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height,
                     0, format, GL_UNSIGNED_BYTE, _bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        _isLoaded = true;
        
        free(_bitmap);
    }
    else {
        // Nothing loaded
        log->error(DGModTexture, "%s: %s", DGMsg210001, stbi_failure_reason());
    }
}

void DGTexture::saveToFile(const char* fileName){
    if (_isLoaded) {
        FILE* fh;
        char fullFileName[DGMaxFileLength - 4];
        
        strncpy(fullFileName, fileName, DGMaxFileLength - 4);
        
        if (_compressionLevel) {
            TEXMainHeader header;
            TEXSubHeader subheader;
            GLint internalformat, size;
             
            // TODO: Must check that the texture is RGB
            // NOTE: Let's try to support alpha channel for these textures
             
            glBindTexture(GL_TEXTURE_2D, _ident);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalformat);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size);
            _bitmap = (GLubyte*)malloc(size * sizeof(GLubyte)); 
            glGetCompressedTexImage(GL_TEXTURE_2D, 0, _bitmap);
            header.compressionLevel = 1;
             
            strcpy(header.name, this->name()); // This is the object name
            header.width = (short)_width;
            header.height = (short)_height;
            header.numTextures = 0;
             
            subheader.cubePosition = 0;
            subheader.depth = (short)_depth;
            subheader.size = (int)size;
            subheader.format = (int)internalformat;
            
            strncat(fullFileName, ".tex", 4);
            
            fh = fopen(fullFileName, "wb");
            fwrite(&TEXIdent, 1, 8, fh);
            fwrite(&header, 1, sizeof(header), fh);
            fwrite(&subheader, 1, sizeof(subheader), fh);
            fwrite(_bitmap, 1, sizeof(GLubyte) * size, fh);
             
            free(_bitmap);
            fclose(fh);
        }
        else {
            unsigned char cGarbage = 0, type, mode;
            short int iGarbage = 0;
            int i;
            GLint size;
            
            strncat(fullFileName, ".tga", 4);
            
            fh = fopen(fullFileName, "wb");
            if (fh == NULL)
                return;
            
            glBindTexture(GL_TEXTURE_2D, _ident);
            
            // We do this in case the texture wasn't loaded
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);
            //glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, &texture_data->depth);
            _depth = 24;
            
            mode = _depth / 8;
            if ((_depth == 24) || (_depth == 32))
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
            
            fwrite(&_width, sizeof(short int), 1, fh);
            fwrite(&_height, sizeof(short int), 1, fh);
            fwrite(&_depth, sizeof(unsigned char), 1, fh);
            
            fwrite(&cGarbage, sizeof(unsigned char), 1, fh);
            
            _depth = mode;
            
            size = _width * _height * _depth;
            _bitmap = (GLubyte*)malloc(size * sizeof(GLubyte)); 
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, _bitmap);
            
            if (mode >= 3) {
                unsigned char aux;

                for (i = 0; i < _width * _height * mode ; i+= mode) {
                    aux = _bitmap[i];
                    _bitmap[i] = _bitmap[i + 2];
                    _bitmap[i + 2] = aux;
                }
            }
            
            fwrite(_bitmap, 1, sizeof(GLubyte) * size, fh);
            fclose(fh);
            
            free(_bitmap);
        }
    }
}

void DGTexture::unload() {
    if (_isLoaded) {
        glDeleteTextures(1, &_ident);
        _usageCount = 0;
        _isLoaded = false;
    }
}
