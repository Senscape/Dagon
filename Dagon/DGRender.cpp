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
    
    if (_fadeTexture)
        delete _fadeTexture;
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGRender::beginDrawing(bool usingTextures) {
    //glEnableClientState(GL_VERTEX_ARRAY);
    
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

void DGRender::drawHelper(int xPosition, int yPosition, bool animate) {
    static float j = 0.0f;
    
    glDisable(GL_LINE_SMOOTH);

    // TODO: Test later if push & pop is necessary at this point
	glPushMatrix();
	glTranslatef(xPosition, yPosition, 0);
    
    if (j > 1.0f) j = 0.0f;
    else j += 0.01f;
    
    if (animate) {
        GLfloat currColor[4];
        glGetFloatv(GL_CURRENT_COLOR, currColor);
        glColor4f(currColor[0], currColor[1], currColor[2], 1.0f - j);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glScalef(1.0f * (j * 2.0f), 1.0f * (j * 2.0f), 0);   
    }
    else glBlendFunc(GL_ONE, GL_ONE);
    
	glVertexPointer(2, GL_FLOAT, 0, _defCursor);
	
	glDrawArrays(GL_LINE_LOOP, 0, (DGDefCursorDetail / 2) + 2);
    
	if (animate) glScalef(0.85f * j, 0.85f * j, 0);
    else glScalef(0.85f, 0.85f, 0);   
    
	glDrawArrays(GL_TRIANGLE_FAN, 0, (DGDefCursorDetail / 2) + 2);
	glPopMatrix();

	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DGRender::drawPolygon(vector<int> withArrayOfCoordinates, unsigned int onFace) {
    const float x = 1.0f;
	const float y = 1.0f;
    
    int i, j;
    int cubeTextureSize = DGDefTexSize;
    int sizeOfArray = (int)withArrayOfCoordinates.size();
	int numCoords = sizeOfArray + (sizeOfArray / 2);
	GLfloat* spotVertCoords = new GLfloat[numCoords];

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
    else {
        // We can safely assume this spot has a color and therefore can use
        // a "helper". Not the most elegant way to do this but, hey, it works.
        DGPoint center = _centerOfPolygon(withArrayOfCoordinates);
        
        // This code is a bit redundant but optimal: the center is only calculated in
        // the required cases. Basically, this projects the calculated point onto the
        // corresponding face.
        float x, y, z;
        
        switch (onFace) {
            case DGNorth:
                x = (GLfloat)center.x / (GLfloat)(cubeTextureSize / 2);
                y = (GLfloat)center.y / (GLfloat)(cubeTextureSize / 2) * -1;
                z = 0.0f;
                break;
            case DGEast:
                x = 0.0f;
                y = (GLfloat)center.y / (GLfloat)(cubeTextureSize / 2) * -1;                
                z = (GLfloat)center.x / (GLfloat)(cubeTextureSize / 2);                  
                break;
            case DGSouth:
                x = (GLfloat)center.x / (GLfloat)(cubeTextureSize / 2) * -1;
				y = (GLfloat)center.y / (GLfloat)(cubeTextureSize / 2) * -1;
				z = 0.0f;
                break;
            case DGWest:
                x = 0.0f;
                y = (GLfloat)center.y / (GLfloat)(cubeTextureSize / 2) * -1;                
                z = (GLfloat)center.x / (GLfloat)(cubeTextureSize / 2) * -1;  
                break;
            case DGUp:
                x = (GLfloat)center.x / (GLfloat)(cubeTextureSize / 2);
                y = 0.0f;                
                z = (GLfloat)center.y / (GLfloat)(cubeTextureSize / 2) * -1;
                break;                 
            case DGDown:
                x = (GLfloat)center.x / (GLfloat)(cubeTextureSize / 2);
                y = 0.0f;
                z = (GLfloat)center.y / (GLfloat)(cubeTextureSize / 2);                
                break;                
        }
        
        DGVector vector = this->project(x, y, z);
        
        if (vector.z < 1.0f) { // Only store coordinates on screen
            DGPoint point;
            
            point.x = (int)vector.x;
            point.y = (int)vector.y;  
            
            _arrayOfHelpers.push_back(point);
        }
    }
    
	glVertexPointer(3, GL_FLOAT, 0, spotVertCoords);
	glDrawArrays(GL_TRIANGLE_FAN, 0, sizeOfArray / 2);
    
    delete spotVertCoords;
    
	glPopMatrix();
}

void DGRender::drawSlide(float* withArrayOfCoordinates) {
    glPushMatrix();
    
	if (_texturesEnabled) {
		GLfloat texCoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	}
    
	glVertexPointer(2, GL_FLOAT, 0, withArrayOfCoordinates);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
	glPopMatrix();
}

void DGRender::endDrawing() {
  	//glDisableClientState(GL_VERTEX_ARRAY);
    
	if (_texturesEnabled) {
		_texturesEnabled = false;
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
    else {
        // Since the color was likely changed, we set white again
        setColor(DGColorWhite);
    }
    
    glFlush();
}

void DGRender::fadeInNextUpdate() {
    _fadeTexture->fadeOut();
}

void DGRender::fadeOutNextUpdate() {
    _fadeTexture->fadeIn();
}

void DGRender::resetFade() {
    _fadeTexture->setFadeLevel(0.0f);
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
        
        glEnable(GL_MULTISAMPLE); // Not sure if this one goes here
	}
    
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
	_blendTexture = new DGTexture(0, 0, 0); // All default values
    _fadeTexture = new DGTexture(1, 1, 0); // Minimal, black texture   
    _fadeTexture->setFadeSpeed(DGFadeFastest);
    
    // NOTE: Here we read the default screen values to calculate the aspect ratio
	for (int i = 0; i < (DGDefCursorDetail + 1) * 2; i += 2) {
		_defCursor[i] = (GLfloat)((.0075 * cosf(i * 1.87f * M_PI / DGDefCursorDetail)) * DGDefDisplayWidth);
		_defCursor[i + 1] = (GLfloat)((.01 * sinf(i * 1.87f * M_PI / DGDefCursorDetail)) * DGDefDisplayHeight);
	}    

    glEnableClientState(GL_VERTEX_ARRAY);
}

void DGRender::setAlpha(float alpha) {
    // NOTE: This resets the current so it should be used with care
    glColor4f(1.0f, 1.0f, 1.0f, alpha);
}

void DGRender::setColor(int color) {
   	uint32_t aux = color;
    
	uint8_t b = (aux & 0x000000ff);
	uint8_t g = (aux & 0x0000ff00) >> 8;
	uint8_t r = (aux & 0x00ff0000) >> 16;
	uint8_t a = (aux & 0xff000000) >> 24;
    
	glColor4f((float)(r / 255.0f), (float)(g / 255.0f), (float)(b / 255.0f), (float)(a / 255.f)); 
}

// FIXME: glReadPixels has an important performace hit on older computers. Improve.
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
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void DGRender::copyScene() {
   	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, config->displayWidth, config->displayHeight, 0); 
}

void DGRender::resetScene() {
    // Check for errors in loop
    if (config->debugMode) {
        GLenum errCode;
        const GLubyte *errString;
        
        if ((errCode = glGetError()) != GL_NO_ERROR) {
            errString = gluErrorString(errCode);
            log->error(DGModRender, "%s: %s", DGMsg220001, errString);
        }
    }
    
    glLoadIdentity();
    _arrayOfHelpers.clear();
}

void DGRender::blendScene() {
	if (_blendNextUpdate) {
		static float j = 0.0f;
		int xStretch = j * (config->displayWidth / 4);
		int yStretch = j * (config->displayHeight / 4);

        // Note the coordinates here are inverted because of the way the screen is captured
        float coords[] = {-xStretch, config->displayHeight + yStretch, 
            config->displayWidth + xStretch, config->displayHeight + yStretch, 
            config->displayWidth + xStretch, -yStretch,
            -xStretch, -yStretch}; 
        
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f - j);
        
		_blendTexture->bind();
		this->drawSlide(coords);
        
		// This should a bit faster than the walk_time factor
		j += 0.015f * config->globalSpeed();
        if (_blendNextUpdate) {
            if (j >= 1.0f) {
                j = 0.0f;
                _blendNextUpdate = false;
            }
        }
	}
}

void DGRender::fadeScene() {
    // FIXME: This is done every time and it sucks
    
    float coords[] = {0, 0, 
        config->displayWidth, 0, 
        config->displayWidth, config->displayHeight,
        0, config->displayHeight};
    
    _fadeTexture->update();
    _fadeTexture->bind();    
    this->setAlpha(_fadeTexture->fadeLevel());
    this->drawSlide(coords);
}

////////////////////////////////////////////////////////////
// Implementation - Splash screen operations
////////////////////////////////////////////////////////////

void DGRender::drawSplash() {
    _splashTexture->bind();
    
    // Note this is inverted
    float coords[] = {0, config->displayHeight, 
        config->displayWidth, config->displayHeight, 
        config->displayWidth, 0,
        0, 0}; 
    
    this->drawSlide(coords);
}

void DGRender::loadSplash() {
    _splashTexture = new DGTexture;
    _splashTexture->loadFromMemory(DGDefSplashBinary);
}

void DGRender::unloadSplash() {
    _splashTexture->unload();
    delete _splashTexture;
}

////////////////////////////////////////////////////////////
// Implementation - Conversion of coordinates
////////////////////////////////////////////////////////////

DGVector DGRender::project(float x, float y, float z) {
    DGVector vector;
    GLdouble winX, winY, winZ;
    
    // Arrays to hold matrix information

    GLdouble modelView[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
	
    // Get window coordinates based on the 3D object
    
    gluProject((GLdouble)x, GLdouble(y), GLdouble(z),
                 modelView, projection, viewport,
                 &winX, &winY, &winZ);
    
    vector.x = (double)winX;
    vector.y = config->displayHeight - (double)winY; // This one must be inverted
    vector.z = (double)winZ;
    
    return vector;
}

DGVector DGRender::unProject(int x, int y) {
    DGVector vector;
    GLdouble objX, objY, objZ;
    
    // Arrays to hold matrix information
    
    GLdouble modelView[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
	
    // Get window coordinates based on the 3D object
    
    gluUnProject((GLdouble)x, GLdouble(y), 0.0f,
               modelView, projection, viewport,
               &objX, &objY, &objZ);
    
    vector.x = objX;
    vector.y = objY;
    vector.z = objZ;
    
    return vector;
}

////////////////////////////////////////////////////////////
// Implementation - Process available helpers
////////////////////////////////////////////////////////////

bool DGRender::beginIteratingHelpers() {
    if (!_arrayOfHelpers.empty()) {
        _itHelper = _arrayOfHelpers.begin();
        
        return true;
    }
    
    return false;
}

DGPoint DGRender::currentHelper() {
    return *_itHelper;
}

bool DGRender::iterateHelpers() {
    _itHelper++;
    
    if (_itHelper == _arrayOfHelpers.end())
        return false;
    else
        return true;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

DGPoint DGRender::_centerOfPolygon(vector<int> arrayOfCoordinates) {
    DGPoint center;    
    int vertex = arrayOfCoordinates.size() / 2;
    
    double area = 0.0;
    double x0 = 0.0; // Current vertex X
    double y0 = 0.0; // Current vertex Y
    double x1 = 0.0; // Next vertex X
    double y1 = 0.0; // Next vertex Y
    double a = 0.0;  // Partial signed area
    
    // For all vertices except last
    int i;
    
    for (i = 0; i < (vertex - 1); i++) {
        x0 = arrayOfCoordinates[i*2 + 0];
        y0 = arrayOfCoordinates[i*2 + 1];
        x1 = arrayOfCoordinates[i*2 + 2];
        y1 = arrayOfCoordinates[i*2 + 3];
        
        a = (x0 * y1) - (x1 * y0);
        area += a;
        
        center.x += (x0 + x1) * a;
        center.y += (y0 + y1) * a;
    }
    
    // Do last vertex
    x0 = arrayOfCoordinates[i*2 + 0];
    y0 = arrayOfCoordinates[i*2 + 1];
    x1 = arrayOfCoordinates[0];
    y1 = arrayOfCoordinates[1];
    
    a = (x0 * y1) - (x1 * y0);
    area += a;
    
    center.x += (x0 + x1) * a;
    center.y += (y0 + y1) * a;
    
    area *= 0.5f;
    center.x /= (6 * area);
    center.y /= (6 * area);
    
    return center;
}
