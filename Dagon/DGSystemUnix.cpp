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

#include <GL/glew.h>
#include <GL/glx.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include "DGAudioManager.h"
#include "DGConfig.h"
#include "DGControl.h"
#include "DGLog.h"
#include "DGPlatform.h"
#include "DGSystem.h"
#include "DGTimerManager.h"
#include "DGVideoManager.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

bool createGLWindow(char* title, int width, int height, int bits,
                    bool fullscreenflag);
void toggleWMFullScreen();

GLvoid killGLWindow();

typedef struct {
    Display *dpy;
    int screen;
    Window win;
    GLXContext ctx;
    XSetWindowAttributes attr;
    bool fs;
    XF86VidModeModeInfo deskMode;
    int x, y;
    unsigned int width, height;
    unsigned int depth;    
} GLWindow;

static int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4, 
    GLX_GREEN_SIZE, 4, 
    GLX_BLUE_SIZE, 4, 
    GLX_DEPTH_SIZE, 16,
    None};

static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER, 
    GLX_RED_SIZE, 4, 
    GLX_GREEN_SIZE, 4, 
    GLX_BLUE_SIZE, 4, 
    GLX_DEPTH_SIZE, 16,
    None };

GLWindow GLWin;

pthread_t tAudioThread;
pthread_t tProfilerThread;
pthread_t tSystemThread;
pthread_t tTimerThread;
pthread_t tVideoThread;

static pthread_mutex_t _audioMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _systemMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _timerMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _videoMutex = PTHREAD_MUTEX_INITIALIZER;

void* _audioThread(void *arg);
void* _profilerThread(void *arg);
void* _systemThread(void *arg);
void* _systemThread(void *arg);
void* _timerThread(void *arg);
void* _videoThread(void *arg);

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

// TODO: At this point the system module should copy the config file
// into the user folder
DGSystem::DGSystem() {  
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();
    
    _isInitialized = false;
    _isRunning = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGSystem::~DGSystem() {
    // The shutdown sequence is performed in the terminate() method
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGSystem::browse(const char* url) {
	char aux[DGMaxFileLength];

	if (config->fullScreen) {
		XIconifyWindow(GLWin.dpy, GLWin.win, GLWin.screen);
	}

	strcpy(aux, "xdg-open ");
	strcat(aux, url);
	system(aux);
}

void DGSystem::createThreads() {
	pthread_create(&tAudioThread, NULL, &_audioThread, NULL);
	pthread_create(&tTimerThread, NULL, &_timerThread, NULL);
	pthread_create(&tVideoThread, NULL, &_videoThread, NULL);

	if (config->debugMode)
		pthread_create(&tProfilerThread, NULL, &_profilerThread, NULL);

	_areThreadsActive = true;
}

void DGSystem::destroyThreads() {
	pthread_mutex_lock(&_audioMutex);
	DGAudioManager::getInstance().terminate();
	pthread_mutex_unlock(&_audioMutex);

	pthread_mutex_lock(&_timerMutex);
	DGTimerManager::getInstance().terminate();
	pthread_mutex_unlock(&_timerMutex);

	pthread_mutex_lock(&_videoMutex);
	DGVideoManager::getInstance().terminate();
	pthread_mutex_unlock(&_videoMutex);

	_areThreadsActive = false;
}

void DGSystem::findPaths(int argc, char* argv[]) {

}

void DGSystem::init() {
    if (!_isInitialized) {
        log->trace(DGModSystem, "%s", DGMsg040000); 

		XInitThreads();
		GLWin.fs = config->fullScreen;
    	createGLWindow("Dagon", config->displayWidth, config->displayHeight, config->displayDepth, GLWin.fs);

		if (GLX_SGI_swap_control) {
			const GLubyte* ProcAddress = reinterpret_cast<const GLubyte*>("glXSwapIntervalSGI");
   			PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = reinterpret_cast<PFNGLXSWAPINTERVALSGIPROC>(glXGetProcAddress(ProcAddress));
    		if (glXSwapIntervalSGI)
        		glXSwapIntervalSGI(config->verticalSync ? 1 : 0);
		}

		// Now we're ready to init the controller instance
    	control = &DGControl::getInstance();
    	control->init();
	control->reshape(config->displayWidth, config->displayHeight);
	control->update();
        _isInitialized = true;

        log->trace(DGModSystem, "%s", DGMsg040001);
    }
    else log->warning(DGModSystem, "%s", DGMsg140002);
}

void DGSystem::resumeThread(int threadID){
    if (_areThreadsActive) {
        switch (threadID) {
            case DGAudioThread:
                pthread_mutex_unlock(&_audioMutex);
                break;
            case DGTimerThread:
                pthread_mutex_unlock(&_timerMutex);
                break;
            case DGVideoThread:
                pthread_mutex_unlock(&_videoMutex);
                break;                
        }
    }
}

void DGSystem::run() {
	if (config->fullScreen)	
		toggleFullScreen();

	char buffer[80];
	static bool isDragging = false;
	static bool isModified = false;
	XEvent event;
    KeySym key;

	int err = pthread_create(&tSystemThread, NULL, &_systemThread, NULL);
        if (err != 0)
            printf("\nCan't create system thread");
	
	_isRunning = true;
	while (_isRunning) {
		// Check for events
		while (XEventsQueued(GLWin.dpy, QueuedAlready) > 0) {
			XNextEvent(GLWin.dpy, &event);
			
			switch (event.type) {
                case Expose:
	                if (event.xexpose.count != 0)
	                    break;
         	        break;				
	            case ConfigureNotify:
					// call resizeGLScene only if our window-size changed
	                if ((event.xconfigure.width != GLWin.width) || 
	                    (event.xconfigure.height != GLWin.height))
	                {
	                    GLWin.width = event.xconfigure.width;
	                    GLWin.height = event.xconfigure.height;
						pthread_mutex_lock(&_systemMutex);
						glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
						control->reshape(GLWin.width, GLWin.height);
						glXMakeCurrent(GLWin.dpy, None, NULL);
						pthread_mutex_unlock(&_systemMutex);
	                }
					break;
				case MotionNotify:
					pthread_mutex_lock(&_systemMutex);
					glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
					if (isDragging)
						control->processMouse(event.xbutton.x, event.xbutton.y, DGMouseEventDrag);
					else
						control->processMouse(event.xbutton.x, event.xbutton.y, DGMouseEventMove);
					glXMakeCurrent(GLWin.dpy, None, NULL);
					pthread_mutex_unlock(&_systemMutex);
					break;
				case ButtonPress:
					pthread_mutex_lock(&_systemMutex);
					glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
					if (event.xbutton.button == 1)
						control->processMouse(event.xbutton.x, event.xbutton.y, DGMouseEventDown);
					else if (event.xbutton.button == 3) 
						control->processMouse(event.xbutton.x, event.xbutton.y, DGMouseEventRightDown);
					glXMakeCurrent(GLWin.dpy, None, NULL);
					pthread_mutex_unlock(&_systemMutex);
					isDragging = true;
					break;
				case ButtonRelease:
					pthread_mutex_lock(&_systemMutex);
					glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
					if (event.xbutton.button == 1)
						control->processMouse(event.xbutton.x, event.xbutton.y, DGMouseEventUp);
					else if (event.xbutton.button == 3) 
						control->processMouse(event.xbutton.x, event.xbutton.y, DGMouseEventRightUp);
					glXMakeCurrent(GLWin.dpy, None, NULL);
					pthread_mutex_unlock(&_systemMutex);
					isDragging = false;
					break;
                		case KeyPress:
					pthread_mutex_lock(&_systemMutex);
					glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
					if (XLookupString(&event.xkey, buffer, 80, &key, 0)) {
						if (isModified) {
							control->processKey(key, DGKeyEventModified);
							isModified = false;
						}
						else
    							control->processKey(key, DGKeyEventDown);
					}
					else {
						if ((key == XK_Control_L) || (key == XK_Control_R))
							isModified = true;
					}
					glXMakeCurrent(GLWin.dpy, None, NULL);
					pthread_mutex_unlock(&_systemMutex);
					break;

                		case KeyRelease:
                    			key = XLookupKeysym(&event.xkey, 0);
					pthread_mutex_lock(&_systemMutex);
					glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
    					control->processKey(key, DGKeyEventUp);
					glXMakeCurrent(GLWin.dpy, None, NULL);
					pthread_mutex_unlock(&_systemMutex);
					break;			
				case ClientMessage:
					// TODO: Simulate ESC key
					this->terminate();
					break;
				default:
					break;
			}
		}
	}
}

void DGSystem::setTitle(const char* title) {

}

void DGSystem::suspendThread(int threadID) {
    if (_areThreadsActive) {
        switch (threadID) {
            case DGAudioThread:
                pthread_mutex_lock(&_audioMutex);
                break;
            case DGTimerThread:
                pthread_mutex_lock(&_timerMutex);
                break;
            case DGVideoThread:
                pthread_mutex_lock(&_videoMutex);
                break;                
        }
    }
}

void DGSystem::terminate() {
	killGLWindow();

	_isRunning = false;
}

void DGSystem::toggleFullScreen() {
	Atom wmState = XInternAtom(GLWin.dpy, "_NET_WM_STATE", False);
        Atom fullscreenAttr = XInternAtom(GLWin.dpy, "_NET_WM_STATE_FULLSCREEN", True);

	XEvent event;
	event.xclient.type=ClientMessage;
	event.xclient.serial = 0;
	event.xclient.send_event=True;
	event.xclient.window=GLWin.win;
	event.xclient.message_type=wmState;
	event.xclient.format=32;
	event.xclient.data.l[0] = 2;
	event.xclient.data.l[1] = fullscreenAttr;
	event.xclient.data.l[2] = 0;

	XSendEvent(GLWin.dpy, DefaultRootWindow(GLWin.dpy), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
}

void DGSystem::update() {
	glXSwapBuffers(GLWin.dpy, GLWin.win);
}

time_t DGSystem::wallTime() {
	// FIXME: Confirm this works with several threads
	return clock();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

bool createGLWindow(char* title, int width, int height, int bits,
                    bool fullscreenflag) {
    XVisualInfo *vi;
    Colormap cmap;
    int dpyWidth, dpyHeight;
    int i;
    int glxMajorVersion, glxMinorVersion;
    int vidModeMajorVersion, vidModeMinorVersion;
    XF86VidModeModeInfo **modes;
    int modeNum;
    int bestMode;
    Atom wmDelete;
    Window winDummy;
    unsigned int borderDummy;
    
    GLWin.fs = fullscreenflag;
    // set best mode to current
    bestMode = 0;
    // get a connection
    GLWin.dpy = XOpenDisplay(0);
    GLWin.screen = DefaultScreen(GLWin.dpy);
    XF86VidModeQueryVersion(GLWin.dpy, &vidModeMajorVersion,
							&vidModeMinorVersion);
    printf("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion,
		   vidModeMinorVersion);
    XF86VidModeGetAllModeLines(GLWin.dpy, GLWin.screen, &modeNum, &modes);
    // save desktop-resolution before switching modes
    GLWin.deskMode = *modes[0];
    // look for mode with requested resolution
    for (i = 0; i < modeNum; i++)
    {
        if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
        {
            bestMode = i;
        }
    }

    // get an appropriate visual
    vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListDbl);
    if (vi == NULL)
    {
        vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListSgl);
        printf("Only Singlebuffered Visual!\n");
    }
    else
    {
        printf("Got Doublebuffered Visual!\n");
    }
    glXQueryVersion(GLWin.dpy, &glxMajorVersion, &glxMinorVersion);
    printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);
    // create a GLX context
    GLWin.ctx = glXCreateContext(GLWin.dpy, vi, 0, GL_TRUE);
    // create a color map
    cmap = XCreateColormap(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
						   vi->visual, AllocNone);
    GLWin.attr.colormap = cmap;
    GLWin.attr.border_pixel = 0;
	
    if (GLWin.fs)
    {
	if (DGConfig::getInstance().forcedFullScreen) {
        	XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, modes[bestMode]);
        	XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
        	dpyWidth = modes[bestMode]->hdisplay;
        	dpyHeight = modes[bestMode]->vdisplay;
        	printf("Resolution %dx%d\n", dpyWidth, dpyHeight);
	}
	else {
        	XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, &GLWin.deskMode);
        	XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
        	dpyWidth = (&GLWin.deskMode)->hdisplay;
        	dpyHeight = (&GLWin.deskMode)->vdisplay;
		DGConfig::getInstance().displayWidth = dpyWidth;
		DGConfig::getInstance().displayHeight = dpyHeight;
        	printf("Resolution %dx%d\n", dpyWidth, dpyHeight);
	}
        XFree(modes);
		
        GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
		ButtonReleaseMask | StructureNotifyMask | PointerMotionMask;
        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
								  0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
								  CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);
        XWarpPointer(GLWin.dpy, None, GLWin.win, 0, 0, 0, 0, 0, 0);
		XMapRaised(GLWin.dpy, GLWin.win);
        XGrabKeyboard(GLWin.dpy, GLWin.win, True, GrabModeAsync,
					  GrabModeAsync, CurrentTime);
        XGrabPointer(GLWin.dpy, GLWin.win, True, ButtonPressMask,
					 GrabModeAsync, GrabModeAsync, GLWin.win, None, CurrentTime);
    }
    else
    {
        // create a window in window mode
        GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
		ButtonReleaseMask | StructureNotifyMask | PointerMotionMask;
        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
								  0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
								  CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);
        // only set window title and handle wm_delete_events if in windowed mode
        wmDelete = XInternAtom(GLWin.dpy, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(GLWin.dpy, GLWin.win, &wmDelete, 1);
        XSetStandardProperties(GLWin.dpy, GLWin.win, title,
							   title, None, NULL, 0, NULL);
        XMapRaised(GLWin.dpy, GLWin.win);
    }       
    // connect the glx-context to the window
    glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);

    XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y,
				 &GLWin.width, &GLWin.height, &borderDummy, &GLWin.depth);
    printf("Depth %d\n", GLWin.depth);
    if (glXIsDirect(GLWin.dpy, GLWin.ctx)) 
        printf("Congrats, you have Direct Rendering!\n");
    else
        printf("Sorry, no Direct Rendering possible!\n");

	// Hide the cursor

	Cursor invisibleCursor;
	Pixmap bitmapNoData;
	XColor black;
	static char noData[] = { 0,0,0,0,0,0,0,0 };
	black.red = black.green = black.blue = 0;

	bitmapNoData = XCreateBitmapFromData(GLWin.dpy, GLWin.win, noData, 8, 8);
	invisibleCursor = XCreatePixmapCursor(GLWin.dpy, bitmapNoData, bitmapNoData, 
                                     &black, &black, 0, 0);
	XDefineCursor(GLWin.dpy,GLWin.win, invisibleCursor);
	XFreeCursor(GLWin.dpy, invisibleCursor);
	
    return true;    
}

GLvoid killGLWindow() {
    if (GLWin.ctx) {
        if (!glXMakeCurrent(GLWin.dpy, None, NULL)) {
            printf("Could not release drawing context.\n");
        }

        glXDestroyContext(GLWin.dpy, GLWin.ctx);
        GLWin.ctx = NULL;
    }

    // switch back to original desktop resolution if we were in fs
    if (GLWin.fs) {
        XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, &GLWin.deskMode);
        XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
    }

    XCloseDisplay(GLWin.dpy);
}

void* _audioThread(void *arg) {
	bool isRunning = true;
	double pause = 10000;

	while (isRunning) {
		pthread_mutex_lock(&_audioMutex);
		isRunning = DGAudioManager::getInstance().update();
		pthread_mutex_unlock(&_audioMutex);
		usleep(pause);
	}

	return 0;
}

void* _profilerThread(void *arg) {
	bool isRunning = true;

	while (isRunning) {
		pthread_mutex_lock(&_systemMutex);
		isRunning = DGControl::getInstance().profiler();
		pthread_mutex_unlock(&_systemMutex);
		sleep(1);
	}

	return 0;
}

void* _systemThread(void *arg) {
	bool isRunning = true;
	double pause = (1.0f / DGConfig::getInstance().framerate) * 1000000;

	while (isRunning) {
		pthread_mutex_lock(&_systemMutex);
		glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
		isRunning = DGControl::getInstance().update();
		glXMakeCurrent(GLWin.dpy, None, NULL);
		pthread_mutex_unlock(&_systemMutex);
		usleep(pause);
	}

	return 0;
}

void* _timerThread(void *arg) {
	bool isRunning = true;
	double pause = 100000;

	while (isRunning) {
		pthread_mutex_lock(&_timerMutex);
		isRunning = DGTimerManager::getInstance().update();
		pthread_mutex_unlock(&_timerMutex);
		usleep(pause);
	}

	return 0;
}

void* _videoThread(void *arg) {
	bool isRunning = true;
	double pause = 10000;

	while (isRunning) {
		pthread_mutex_lock(&_videoMutex);
		isRunning = DGVideoManager::getInstance().update();
		pthread_mutex_unlock(&_videoMutex);
		usleep(pause);
	}

	return 0;
}

