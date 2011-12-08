/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGCamera.h"
#include "DGCommonGL.h"
#include "DGConfig.h"
#include "DGEffects.h"
#include "DGLog.h"
#include "DGMemory.h"
#include "DGRender.h"
#include "DGTexture.h"
#include "DGVideo.h"

#include <GL\GLU.h>
#define N_DUST 1000
#define DUST_FACTOR 32767.0f

DGObject* dustTexture;

static struct particle {
	GLfloat x,y,z;
	GLfloat r,g,b;
	GLfloat xd,yd,zd;
	GLfloat cs;
} p[N_DUST];

int randomize(DGBool full) {
    int n = 32768;
    if (full)
        while (n > 32767)
            n = rand() / 32767;
    else
        while (n < 0 || n > 32767)
            n = rand() / 32767;
    return n;
}

void DGRenderDrawDust() {
    int i;
    float s = 0.0015f;
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    DGTextureBind(dustTexture);
    for (i = 0; i < N_DUST; i++)
    {
        p[i].x+=p[i].xd/100.0f;
        p[i].y+=p[i].yd/100.0f;
        p[i].z+=p[i].zd/100.0f;
        if (p[i].y<=-0.5f)
        {   // This should be defined according to the size of the cube
            p[i].xd=-(randomize(DG_YES)/DUST_FACTOR-0.5f)/200.0f;
            p[i].zd=-(randomize(DG_YES)/DUST_FACTOR-0.5f)/200.0f;
            p[i].yd=-randomize(DG_YES)/DUST_FACTOR/100.0f;
            p[i].x=(randomize(DG_YES)/DUST_FACTOR-0.5f);
            p[i].y=randomize(DG_NO)/DUST_FACTOR;
            p[i].z=(randomize(DG_YES)/DUST_FACTOR-0.5f);
        }

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(p[i].x,p[i].y,p[i].z+s);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(p[i].x,p[i].y+s,p[i].z+s);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(p[i].x+s,p[i].y+s,p[i].z+s);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(p[i].x+s,p[i].y,p[i].z+s);
        glEnd();
    }
}

// TODO: This should obviously go in the Texture manager
DGObject* blendTexture;

static DGBool blendNextUpdate;
static int cubeTextureSize;
static DGBool texturesEnabled;

#define NUM_VERTS 40
GLfloat circle[(NUM_VERTS * 2) + 2];

DGBool _isExtensionSupported(const char *extension);
static DGBool supports_shaders = DG_NO;

unsigned int frameBuffer;
unsigned int frameBufferDepth;
unsigned int frameBufferTexture;	



// GH: Init the framebuffer depth 
void DGInitFrameBufferDepth()
{
	glGenRenderbuffersEXT(1, &frameBufferDepth);
	// GH: Init the depth setting
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, frameBufferDepth);
	// GH: set the size of the depth
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, DGConfig.display_width, DGConfig.display_height);
	// GH: Actually set it as the depth for the framebuffer object
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, frameBufferDepth); 
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	// GH: End the frame buffer depth
}

// GH: Create the framebuffer texture
void DGInitFrameBufferTexture()
{
	// GH: Init texture creation
	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
  
	glBindTexture(GL_TEXTURE_2D, 0);  
}

void DGInitFrameBuffer()
{
	DGInitFrameBufferDepth();
	DGInitFrameBufferTexture();

	glGenFramebuffersEXT(1, &frameBuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, frameBufferTexture, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, frameBufferDepth); 

/*	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);   
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) 
	{  
		DGLogError(DG_MOD_RENDER, "Failed to create the framebuffer correctly");
	}  */
  
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); 
}


void DGRenderInitialize() {
	int i;
	const GLubyte* version = glGetString(GL_VERSION);
	
	DGLogTrace(DG_MOD_RENDER, "Initializing render system...");
	DGLogInfo(DG_MOD_RENDER, "OpenGL version: %s", version);
	
	/*GLint texSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
	 
	GL_MAX_TEXTURE_IMAGE_UNITS // For the cache
	*/
	
	glewInit();
	
	if (glewIsSupported("GL_VERSION_2_0")) {
		DGEffectsInit();
        DGEffectsSetGamma(2.5f);
		supports_shaders = DG_YES;
	}
	else {
		DGLogWarning(DG_MOD_RENDER, "Visual effects not supported on this system");
		supports_shaders = DG_NO;
	}

	//glEnable(GL_TEXTURE_2D);	
	
	
	glEnable(GL_BLEND);
	if (DGConfig.antialiasing) {
		// TODO: Some of these options may be introducing black lines
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	}
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	DGInitFrameBuffer();
	
	/*glDisable(GL_DEPTH_TEST);
	glDisable(GL_MULTISAMPLE);
	glDisable(GL_DITHER);*/
	
	//glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	cubeTextureSize = DG_DEF_TEXSIZE;
	texturesEnabled = DG_NO;
	
	blendNextUpdate = DG_NO;
	
	blendTexture = DGTextureNew("", 0);   // Please note the compression level here
	DGTextureClear(blendTexture, 0, 0, 0);
	
	for (i = 0; i < (NUM_VERTS+1)*2; i+=2) {
		circle[i] = (GLfloat)((.0075 * cosf(i*2*3.14159/NUM_VERTS)) * DG_DEF_DISPLAYWIDTH);
		circle[i+1] = (GLfloat)((.01 * sinf(i*2*3.14159/NUM_VERTS)) * DG_DEF_DISPLAYHEIGHT);
	}
    
    for (i = 0; i < N_DUST; i++) {
        p[i].xd=-(randomize(DG_YES)/DUST_FACTOR-0.5f)/200.0f;
        p[i].zd=-(randomize(DG_YES)/DUST_FACTOR-0.5f)/200.0f;
        p[i].yd=-randomize(DG_YES)/DUST_FACTOR/100.0f;
        p[i].x=(randomize(DG_YES)/DUST_FACTOR-0.5f);
        p[i].y=randomize(DG_NO)/DUST_FACTOR;
        p[i].z=(randomize(DG_YES)/DUST_FACTOR-0.5f);
    }
    
    dustTexture = DGTextureNew("", 0);
    DGTextureLoadFromMemory(dustTexture, _binary_def_particle_start);
}

void DGRenderBegin(DGBool usingTextures) {    
	glEnableClientState(GL_VERTEX_ARRAY);
	if (usingTextures) {
		texturesEnabled = DG_YES;
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
		// Need to define "rendering states" to properly handle this: ie, normal, blending, etc.
		if (DGConfig.show_spots)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		if (supports_shaders) 
			DGEffectsPause();
		glBlendFunc(GL_ONE, GL_ZERO);
	}
}

void DGRenderEnd() {
	glDisableClientState(GL_VERTEX_ARRAY);
	
	if (texturesEnabled) {
		texturesEnabled = DG_NO;
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else {
		if (supports_shaders)
			DGEffectsPlay();
	}
}

void DGRenderBlendNextUpdate() {
	blendNextUpdate = DG_YES;
}

void DGRenderClearScene() {	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glFlush();
}

void DGRenderDrawCursor(short xPosition, short yPosition) {
	/*short cursor_size = 64;
	short coords[] = {0, 0, cursor_size, 0, 
	 cursor_size, cursor_size, 0, cursor_size};
	 
	dg_camera_set_ortho();
	glPushMatrix();
	glTranslatef(x, y, 0);
	dg_render_begin(DG_YES);
	dg_render_draw_quad(coords, DG_NO);
	dg_render_end();
	glPopMatrix();
	dg_camera_set_perspective();*/
	
	glDisable(GL_LINE_SMOOTH);
	DGCameraSetOrthoView();
	glPushMatrix();
	glTranslatef(xPosition - (circle[((NUM_VERTS * 2) + 2) / 2] / 2), 
				 yPosition - (circle[((NUM_VERTS * 2) + 2) / 2] / 2), 0);
	glVertexPointer(2, GL_FLOAT, 0, circle);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDrawArrays(GL_LINE_LOOP, 0, (NUM_VERTS / 2) + 2);
	glScalef(0.85f, 0.85f, 0);
	glBlendFunc(GL_ONE, GL_ONE);
	glDrawArrays(GL_TRIANGLE_FAN, 0, (NUM_VERTS / 2) + 2);
	glPopMatrix();
	DGCameraSetPerspectiveView();
	glEnable(GL_LINE_SMOOTH);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DGRenderDrawPolygon(short* withArrayOfCoordinates, short sizeOfArray, short onFace, DGBool isInverted) {
	const float x = 1.0f;
	const float y = 1.0f;
	
	int i, j;
	int numVertices = sizeOfArray + (sizeOfArray / 2);
	GLfloat* spotVertCoords = DGAlloc(numVertices * sizeof(int));
	int invertY;
		
	if (isInverted)
		invertY = -1;
	else
		invertY = 1;

	glPushMatrix();
	
	switch (onFace) {
		case DG_FRONT:
			glTranslatef(-x, y, -x);
			for (i = 0, j = 0; i < sizeOfArray; i+=2, j+=3) {
				spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize/2);			// Size is divided in half because of the way we draw the cube
				spotVertCoords[j+1] = (GLfloat)withArrayOfCoordinates[i+1] / (GLfloat)(cubeTextureSize/2) * -1;	// We must invert the coordinates in some cases
				spotVertCoords[j+2] = 0.0f;
			}
			break;
		case DG_RIGHT:
			glTranslatef(x, y, -x);
			for (i = 0, j = 0; i < sizeOfArray; i+=2, j+=3) {
				spotVertCoords[j] = 0.0f;
				spotVertCoords[j+1] = (GLfloat)withArrayOfCoordinates[i+1] / (GLfloat)(cubeTextureSize/2) * -1;
				spotVertCoords[j+2] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize/2);
			}
			break;
		case DG_BACK:
			glTranslatef(x, y, x);
			for (i = 0, j = 0; i < sizeOfArray; i+=2, j+=3) {
				spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize/2) * -1;
				spotVertCoords[j+1] = (GLfloat)withArrayOfCoordinates[i+1] / (GLfloat)(cubeTextureSize/2) * -1;
				spotVertCoords[j+2] = 0.0f;
			}			
			break;
		case DG_LEFT:
			glTranslatef(-x, y, x);
			for (i = 0, j = 0; i < sizeOfArray; i+=2, j+=3) {
				spotVertCoords[j] = 0.0f;
				spotVertCoords[j+1] = (GLfloat)withArrayOfCoordinates[i+1] / (GLfloat)(cubeTextureSize/2) * -1;
				spotVertCoords[j+2] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize/2) * -1;
			}
			break;
		case DG_TOP:
			glTranslatef(-x, y, x);
			for (i = 0, j = 0; i < sizeOfArray; i+=2, j+=3) {
				spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize/2);
				spotVertCoords[j+1] = 0.0f;
				spotVertCoords[j+2] = (GLfloat)withArrayOfCoordinates[i+1] / (GLfloat)(cubeTextureSize/2) * -1;
			}
			break;
		case DG_BOTTOM:
			glTranslatef(-x, -y, -x);
			for (i = 0, j = 0; i < sizeOfArray; i+=2, j+=3) {
				spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize/2);
				spotVertCoords[j+1] = 0.0f;
				spotVertCoords[j+2] = (GLfloat)withArrayOfCoordinates[i+1] / (GLfloat)(cubeTextureSize/2);
			}
			break;				
	}
	
	if (texturesEnabled) {
		float texU = (float)1 / (cubeTextureSize * 2);
		float texV = (float)((cubeTextureSize * 2) - 1) / (cubeTextureSize * 2);

		GLfloat texCoords[] = {texU, (texU*invertY), texV, (texU*invertY), texV, (texV*invertY), texU, (texV*invertY)};
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	}
	glVertexPointer(3, GL_FLOAT, 0, spotVertCoords);
	glDrawArrays(GL_TRIANGLE_FAN, 0, sizeOfArray / 2);
	
	glPopMatrix();
}

void DGRenderDrawOverlay(short* withArrayOfCoordinates, DGBool isInverted) {
	int invertY;
		
	if (isInverted)
		invertY = -1;
	else
		invertY = 1;

	glPushMatrix();
	
	if (texturesEnabled) {
		GLfloat texCoords[] = {0.0f, (0.0f*invertY), 1.0f, (0.0f*invertY), 1.0f, (1.0f*invertY), 0.0f, (1.0f*invertY)};
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	}
	glVertexPointer(2, GL_SHORT, 0, withArrayOfCoordinates);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	glPopMatrix();
}

// TODO: This is unnecessary... let's try to avoid this extra method
void DGRenderCopyScene() {
	DGTextureBind(blendTexture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, DGConfig.display_width, DGConfig.display_height, 0);
}

void DGRenderShowCutscene(DGObject* withVideo) {
	DGFrame* frame = DGVideoGetFrame(withVideo);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->width, frame->height, GL_BGR, GL_UNSIGNED_BYTE, frame->data);
	
	// Alpha is not supported for videos, so we set the next drawing cycle to be fully opaque
	glBlendFunc(GL_ONE, GL_ZERO);
}

void DGRenderSaveScene() {
	// Should move this elsewhere, but what the heck...
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];
	
	time(&rawtime);
	timeinfo = localtime (&rawtime);
	
	strftime(buffer, 80, "snap-%Y-%m-%d-%Hh%Mm%Ss.tga", timeinfo);
	
    DGTextureSaveToFile(blendTexture, buffer);
}

void DGRenderSetSplash() {
    DGTextureLoadFromMemory(blendTexture, _binary_def_splash_start);
}

void DGRenderDrawSplash() {
    static float j = 0.0f;
	
	/*short width = config.display_width;
	float factor = (float)dg_texture_width(blendTexture) / (float)config.display_width;
    short height = dg_texture_height(blendTexture) / factor;
	short off = config.display_height - height;*/
	
    short coords[] = {0, 0, DGConfig.display_width, 0, 
        DGConfig.display_width, DGConfig.display_height, 0, DGConfig.display_height};
    
    if (supports_shaders)
		DGEffectsPause();
    
    glColor4f(1.0f, 1.0f, 1.0f, j);
    
    DGTextureBind(blendTexture);
    DGRenderDrawOverlay(coords, DG_NO);
    
    if (j < 1.0f)
        j += 0.01f * DGConfig.global_speed;
    
    if (supports_shaders)
		DGEffectsPlay();
}

void DGRenderTerminate() {
	if (supports_shaders)
		DGEffectsTerminate();
}

void DGRenderSetColor(int color) {
	uint32_t aux = color;
	
	uint8_t b = (aux & 0x000000ff);
	uint8_t g = (aux & 0x0000ff00) >> 8;
	uint8_t r = (aux & 0x00ff0000) >> 16;
	uint8_t a = (aux & 0xff000000) >> 24;
	
	glColor4f((float)(r/255.0f), (float)(g/255.0f), (float)(b/255.0f), (float)(a/255.f));
}

int	DGRenderTestColor(int xPosition, int yPosition) {
	GLubyte pixel[3];
	GLint r, g, b;
	uint32_t aux;
	
	glReadPixels(xPosition, yPosition, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
	
	r = (GLint)pixel[0];
	g = (GLint)pixel[1];
	b = (GLint)pixel[2];
	
	aux = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
	
	return aux;
}

void DGRenderUpdateScene() {
	GLenum errCode;
	const GLubyte *errString;
	
    if (DGConfig.dust)
        DGRenderDrawDust();
    
	if (supports_shaders) {
		// Badly placed...
		DGEffectsSetBlur(DGCameraDeltaX(), DGCameraDeltaY());
		// TODO: Noise should remain on top of the blend
		DGEffectsPause();
	}

	if (blendNextUpdate) {
		static float j = 0.0f;
		short xStretch = (short)(j * (DGConfig.display_width/4));
		short yStretch = (short)(j * (DGConfig.display_height/4));
		short coords[] = {-xStretch, -yStretch, DGConfig.display_width+xStretch, -yStretch, 
			DGConfig.display_width+xStretch, DGConfig.display_height+yStretch, -xStretch, DGConfig.display_height+yStretch}; 

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f - j);
		
		DGCameraSetOrthoView();
		
		DGRenderBegin(DG_YES);
		
		DGTextureBind(blendTexture);
	//	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
		DGRenderDrawOverlay(coords, DG_NO);
		
//		glBindTexture(GL_TEXTURE_2D, 0);
		DGRenderEnd();
		
		
		DGCameraSetPerspectiveView();
		
		// This should a bit faster than the walk_time factor
		j += 0.015f * DGConfig.global_speed;
		if (j >= 1.0f) {
			j = 0.0f;
			blendNextUpdate = DG_NO;
		}
	}

	if (supports_shaders)
		DGEffectsPlay();
	
	// Check for errors in loop
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		DGLogError(DG_MOD_RENDER, "%s %s", MSG220001, errString);
	}
}

DGBool _isExtensionSupported(const char *extension) {	
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;
	/* Extension names should not have spaces. */
	
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;
	
	extensions = glGetString(GL_EXTENSIONS);
	
	/* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
	
	start = extensions;
	for (;;) {
		where = (GLubyte *) strstr((const char *) start, extension);
		if (!where)
			break;
		
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return DG_YES;
		
		start = terminator;
	}
	
	return DG_NO;
}
