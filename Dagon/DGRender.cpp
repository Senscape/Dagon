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

#include "DGConfig.h"
#include "DGLog.h"
#include "DGRender.h"
#include "DGTexture.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGRender::DGRender() {
    config = &DGConfig::getInstance();
    log = &DGLog::getInstance();
    
    _blendNextUpdate = false;
	_texturesEnabled = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGRender::~DGRender() {
    if (_blendTexture)
        delete _blendTexture;
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGRender::beginDrawing(bool usingTextures) {
    glEnableClientState(GL_VERTEX_ARRAY);
    
	if (usingTextures) {
		_texturesEnabled = true;
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
        
		// Need to define "rendering states" to properly handle this: ie, normal, blending, etc.
		if (config->showSpots)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glBlendFunc(GL_ONE, GL_ZERO);
	}
}

void DGRender::blendNextUpdate() {
    _blendTexture->bind();
    this->copyScene();
    _blendNextUpdate = true;
}

void DGRender::drawCursor(int xPosition, int yPosition) {
    glDisable(GL_LINE_SMOOTH);

    // TODO: Test later if push & pop is necessary at this point
	glPushMatrix();
	glTranslatef(xPosition, yPosition, 0);
    
	glVertexPointer(2, GL_FLOAT, 0, _defCursor);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDrawArrays(GL_LINE_LOOP, 0, (DGDefCursorDetail / 2) + 2);
	glScalef(0.85f, 0.85f, 0);
	glBlendFunc(GL_ONE, GL_ONE);
	glDrawArrays(GL_TRIANGLE_FAN, 0, (DGDefCursorDetail / 2) + 2);
	glPopMatrix();

	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DGRender::drawPolygon(std::vector<int> withArrayOfCoordinates, unsigned int onFace) {
    const float x = 1.0f;
	const float y = 1.0f;
    
    int i, j;
    int cubeTextureSize = DGDefTexSize;
    int sizeOfArray = (int)withArrayOfCoordinates.size();
	int numVertices = sizeOfArray + (sizeOfArray / 2);
	GLfloat* spotVertCoords = new GLfloat[numVertices];

	glPushMatrix();
    
	switch (onFace) {
		case DGNorth:
			glTranslatef(-x, y, -x);
			for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
				spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize / 2); // Size is divided in half because of the way we draw the cube
				spotVertCoords[j + 1] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize / 2) * -1; // We must invert the coordinates in some cases
				spotVertCoords[j + 2] = 0.0f;
			}
			break;
		case DGEast:
			glTranslatef(x, y, -x);
			for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
				spotVertCoords[j] = 0.0f;
				spotVertCoords[j + 1] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize / 2) * -1;
				spotVertCoords[j + 2] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize / 2);
			}
			break;
		case DGSouth:
			glTranslatef(x, y, x);
			for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
				spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize / 2) * -1;
				spotVertCoords[j + 1] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize / 2) * -1;
				spotVertCoords[j + 2] = 0.0f;
			}			
			break;
		case DGWest:
			glTranslatef(-x, y, x);
			for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
				spotVertCoords[j] = 0.0f;
				spotVertCoords[j + 1] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize/ 2 ) * -1;
				spotVertCoords[j + 2] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize / 2) * -1;
			}
			break;
		case DGUp:
			glTranslatef(-x, y, x);
			for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
				spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize / 2);
				spotVertCoords[j + 1] = 0.0f;
				spotVertCoords[j + 2] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize / 2) * -1;
			}
			break;
		case DGDown:
			glTranslatef(-x, -y, -x);
			for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
				spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize / 2);
				spotVertCoords[j + 1] = 0.0f;
				spotVertCoords[j + 2] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize / 2);
			}
			break;				
	}
    
	if (_texturesEnabled) {
		float texU = (float)1 / (cubeTextureSize * 2);
		float texV = (float)((cubeTextureSize * 2) - 1) / (cubeTextureSize * 2);
        
		GLfloat texCoords[] = {texU, texU, texV, texU, texV, texV, texU, texV};
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	}
    
	glVertexPointer(3, GL_FLOAT, 0, spotVertCoords);
	glDrawArrays(GL_TRIANGLE_FAN, 0, sizeOfArray / 2);
    
    delete spotVertCoords;
    
	glPopMatrix();
}

void DGRender::drawSlide(int* withArrayOfCoordinates) {
    glPushMatrix();
    
	if (_texturesEnabled) {
		GLfloat texCoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	}
    
	glVertexPointer(2, GL_INT, 0, withArrayOfCoordinates);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
	glPopMatrix();
}

void DGRender::endDrawing() {
  	glDisableClientState(GL_VERTEX_ARRAY);
    
	if (_texturesEnabled) {
		_texturesEnabled = false;
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
    else {
        // Since the color was likely changed, we set white again
        setColor(DGColorWhite);
    }
}

void DGRender::init() {
	const GLubyte* version = glGetString(GL_VERSION);

	log->trace(DGModRender, "%s", DGMsg020000);
	log->info(DGModRender, "%s: %s", DGMsg020001, version);
    
	glewInit();
    
	if (glewIsSupported("GL_VERSION_2_0")) {
		_shadersEnabled = true;
	}
	else {
		log->trace(DGModRender, "%s", DGMsg020002);
		_shadersEnabled = false;
	}
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_MULTISAMPLE);
	glDisable(GL_DITHER);
    
	if (config->antialiasing) {
		// TODO: Some of these options may be introducing black lines
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	}
    
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
	_blendTexture = new DGTexture(0, 0, 0); // All default values
    
    // NOTE: Here we read the default screen values to calculate the aspect ratio
	for (int i = 0; i < (DGDefCursorDetail + 1) * 2; i += 2) {
		_defCursor[i] = (GLfloat)((.0075 * cosf(i * 2 * 3.14159 / DGDefCursorDetail)) * DGDefDisplayWidth);
		_defCursor[i + 1] = (GLfloat)((.01 * sinf(i * 2 * 3.14159 / DGDefCursorDetail)) * DGDefDisplayHeight);
	}    
}

void DGRender::setColor(int color) {
   	uint32_t aux = color;
    
	uint8_t b = (aux & 0x000000ff);
	uint8_t g = (aux & 0x0000ff00) >> 8;
	uint8_t r = (aux & 0x00ff0000) >> 16;
	uint8_t a = (aux & 0xff000000) >> 24;
    
	glColor4f((float)(r / 255.0f), (float)(g / 255.0f), (float)(b / 255.0f), (float)(a / 255.f)); 
}

int	DGRender::testColor(int xPosition, int yPosition) {
    GLubyte pixel[3];
	GLint r, g, b;
	uint32_t aux;
    
    // Note we flip the Y coordinate here because only the orthogonal projection is flipped
	glReadPixels(xPosition, config->displayHeight - yPosition, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    
	r = (GLint)pixel[0];
	g = (GLint)pixel[1];
	b = (GLint)pixel[2];
    
	aux = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
    
	return aux;
}

////////////////////////////////////////////////////////////
// Implementation - Scene operations
////////////////////////////////////////////////////////////

void DGRender::clearScene() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
}

void DGRender::copyScene() {
   	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, config->displayWidth, config->displayHeight, 0); 
}

void DGRender::updateScene() {
	if (_blendNextUpdate) {
		static float j = 0.0f;
		int xStretch = j * (config->displayWidth / 4);
		int yStretch = j * (config->displayHeight / 4);
		int coords[] = {-xStretch, -yStretch, config->displayWidth + xStretch, -yStretch, 
			config->displayWidth + xStretch, config->displayHeight + yStretch, -xStretch, config->displayHeight + yStretch}; 
        
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f - j);
        
		//DGCameraSetOrthoView();
        
		this->beginDrawing(true);
		_blendTexture->bind();
		this->drawSlide(coords);
		this->endDrawing();
        
		//DGCameraSetPerspectiveView();
        
		// This should a bit faster than the walk_time factor
		j += 0.015f * config->globalSpeed();
		if (j >= 1.0f) {
			j = 0.0f;
			_blendNextUpdate = false;
		}
	}
    
	// Check for errors in loop
    if (config->debugMode) {
        GLenum errCode;
        const GLubyte *errString;
        
        if ((errCode = glGetError()) != GL_NO_ERROR) {
            errString = gluErrorString(errCode);
            log->error(DGModRender, "%s: %s", DGMsg220001, errString);
        }
    }
}
