/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGCommonGL."
#include "DGConfig.h"
#include "DGControl.h"
#include "DGLog.h"
#include "DGSystem.h"

#ifndef DG_USE_SDL


DG_BOOL createGLWindow(char* title, int width, int height, int bits,
                    DG_BOOL fullscreenflag);

GLvoid killGLWindow(GLvoid);

typedef struct {
    Display *dpy;
    int screen;
    Window win;
    GLXContext ctx;
    XSetWindowAttributes attr;
    DG_BOOL fs;
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
DG_BOOL done;

void DGSystemInitialize() {
	DGLogTrace(DG_MOD_SYSTEM, "Initializing system interface...");

	GLWin.fs = config.fullscreen;
    createGLWindow("Dagon", DGConfig.display_width, DGConfig.display_height, 24, GLWin.fs);
}

void DGSystemRun() {
	XEvent event;
    KeySym key;
	
	while (!done) {
		/* Check for events */
		while (XPending(GLWin.dpy) > 0) {
			XNextEvent(GLWin.dpy, &event);
			
			switch (event.type) {
                case Expose:
	                if (event.xexpose.count != 0)
	                    break;
                    DGControlUpdate();
         	        break;				
	            case ConfigureNotify:
					/* call resizeGLScene only if our window-size changed */
	                if ((event.xconfigure.width != GLWin.width) || 
	                    (event.xconfigure.height != GLWin.height))
	                {
	                    GLWin.width = event.xconfigure.width;
	                    GLWin.height = event.xconfigure.height;
						DGConfig.display_width = GLWin.width;
						DGConfig.display_height = GLWin.height;
	                }
					break;
				case MotionNotify:
					DGControlProcessMouse(event.xbutton.x, DGConfig.display_height - event.xbutton.y, DG_NO);
					break;
				case ButtonPress:
					break;
				case ButtonRelease:
					DGControlProcess_mouse(event.xbutton.x, DGConfig.display_height - event.xbutton.y, DG_YES);
					break;
                case KeyPress:
                    key = XLookupKeysym(&event.xkey, 0);
					switch (key) {
						case XK_Escape:
							done = DG_YES;
							DGControlTerminate();
							break;
						case XK_F1:
							killGLWindow();
							GLWin.fs = !GLWin.fs;
							createGLWindow("Dagon", DGConfig.display_width, DGConfig.display_height, 24, GLWin.fs);
							break;
					}
					break;
				case ClientMessage:
					done = DG_YES;
					DGSystemTerminate();
					break;
				default:
					break;
			}
		}
		
		DGControlUpdate();
	}
}

void DGSystemTerminate() {
	killGLWindow();
}

void DGSystemUpdate() {
	glXSwapBuffers(GLWin.dpy, GLWin.win);
}

DG_BOOL createGLWindow(char* title, int width, int height, int bits,
                    DGBool fullscreenflag) {
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
    /* set best mode to current */
    bestMode = 0;
    /* get a connection */
    GLWin.dpy = XOpenDisplay(0);
    GLWin.screen = DefaultScreen(GLWin.dpy);
    XF86VidModeQueryVersion(GLWin.dpy, &vidModeMajorVersion,
							&vidModeMinorVersion);
    printf("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion,
		   vidModeMinorVersion);
    XF86VidModeGetAllModeLines(GLWin.dpy, GLWin.screen, &modeNum, &modes);
    /* save desktop-resolution before switching modes */
    GLWin.deskMode = *modes[0];
    /* look for mode with requested resolution */
    for (i = 0; i < modeNum; i++)
    {
        if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
        {
            bestMode = i;
        }
    }
    /* get an appropriate visual */
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
    /* create a GLX context */
    GLWin.ctx = glXCreateContext(GLWin.dpy, vi, 0, GL_TRUE);
    /* create a color map */
    cmap = XCreateColormap(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
						   vi->visual, AllocNone);
    GLWin.attr.colormap = cmap;
    GLWin.attr.border_pixel = 0;
	
    if (GLWin.fs)
    {
        XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, modes[bestMode]);
        XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
        dpyWidth = modes[bestMode]->hdisplay;
        dpyHeight = modes[bestMode]->vdisplay;
        printf("Resolution %dx%d\n", dpyWidth, dpyHeight);
        XFree(modes);
		
        /* create a fullscreen window */
        GLWin.attr.override_redirect = True;
        GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
		ButtonReleaseMask | StructureNotifyMask | PointerMotionMask;
        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
								  0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
								  CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
								  &GLWin.attr);
        XWarpPointer(GLWin.dpy, None, GLWin.win, 0, 0, 0, 0, 0, 0);
		XMapRaised(GLWin.dpy, GLWin.win);
        XGrabKeyboard(GLWin.dpy, GLWin.win, True, GrabModeAsync,
					  GrabModeAsync, CurrentTime);
        XGrabPointer(GLWin.dpy, GLWin.win, True, ButtonPressMask,
					 GrabModeAsync, GrabModeAsync, GLWin.win, None, CurrentTime);
    }
    else
    {
        /* create a window in window mode*/
        GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
		ButtonReleaseMask | StructureNotifyMask | PointerMotionMask;
        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
								  0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
								  CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);
        /* only set window title and handle wm_delete_events if in windowed mode */
        wmDelete = XInternAtom(GLWin.dpy, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(GLWin.dpy, GLWin.win, &wmDelete, 1);
        XSetStandardProperties(GLWin.dpy, GLWin.win, title,
							   title, None, NULL, 0, NULL);
        XMapRaised(GLWin.dpy, GLWin.win);
    }       
    /* connect the glx-context to the window */
    glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
    XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y,
				 &GLWin.width, &GLWin.height, &borderDummy, &GLWin.depth);
    printf("Depth %d\n", GLWin.depth);
    if (glXIsDirect(GLWin.dpy, GLWin.ctx)) 
        printf("Congrats, you have Direct Rendering!\n");
    else
        printf("Sorry, no Direct Rendering possible!\n");
	
    return DG_YES;    
}

GLvoid killGLWindow(GLvoid)
{
    if (GLWin.ctx)
    {
        if (!glXMakeCurrent(GLWin.dpy, None, NULL))
        {
            printf("Could not release drawing context.\n");
        }
        glXDestroyContext(GLWin.dpy, GLWin.ctx);
        GLWin.ctx = NULL;
    }
    /* switch back to original desktop resolution if we were in fs */
    if (GLWin.fs)
    {
        XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, &GLWin.deskMode);
        XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
    }
    XCloseDisplay(GLWin.dpy);
}

#endif

