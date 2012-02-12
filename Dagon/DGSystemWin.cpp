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
#include <string.h>
#include <Windows.h>
#include "DGDefines.h"
#include "DGLanguage.h"
#include "DGPlatform.h"
#include "DGSystem.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// Tell the Windows subsystem that we shouldn't create a console
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

// These are private in order to keep a clean and portable
// header

HWND g_hWnd = NULL;
HDC g_hDC = NULL;
HGLRC g_hRC = NULL;

DWORD WINAPI _systemThread(LPVOID lpParam);
LRESULT CALLBACK _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Get pointers to functions for setting vertical sync
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);

PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;

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

void DGSystem::findPaths(int argc, char* argv[]) {
    // Implement
}

void DGSystem::init() {
    if (!_isInitialized) {
        log->trace(DGModSystem, "========================================");
        log->trace(DGModSystem, "%s", DGMsg040000);        

        // Prepare the string to set the window title
        WCHAR title[DGMaxPathLength];
        MultiByteToWideChar(0, 0, config->script(), DGMaxFileLength, title, DGMaxFileLength);
        
        WNDCLASSEX winClass;        
        winClass.lpszClassName = L"DG_WINDOWS_CLASS";
        winClass.cbSize        = sizeof(WNDCLASSEX);
        winClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        winClass.lpfnWndProc   = _WindowProc;
        winClass.hInstance     = NULL;
        winClass.hIcon         = LoadIcon(NULL, (LPCTSTR)IDI_APPLICATION);
        winClass.hIconSm       = LoadIcon(NULL, (LPCTSTR)IDI_APPLICATION);
        winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
        winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        winClass.lpszMenuName  = NULL;
        winClass.cbClsExtra    = 0;
        winClass.cbWndExtra    = 0;
        
        if (!RegisterClassEx(&winClass))
            return;
        
        // Now we create the actual window
        g_hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"DG_WINDOWS_CLASS", title,
                                WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 
                                config->displayWidth, config->displayHeight, NULL, NULL, NULL, NULL );
        
        if (g_hWnd == NULL)
            return;
        
        ShowWindow(g_hWnd, SW_SHOWNORMAL);
        UpdateWindow(g_hWnd);
        
        // Setup the OpenGL pixel format
        GLuint PixelFormat;
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        
        pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW |PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 16;
        pfd.cDepthBits = 16;
        
        g_hDC = GetDC(g_hWnd);
        PixelFormat = ChoosePixelFormat(g_hDC, &pfd);
        SetPixelFormat( g_hDC, PixelFormat, &pfd);
        g_hRC = wglCreateContext(g_hDC);
        wglMakeCurrent(g_hDC, g_hRC);
        
        // Now we're ready to init the controller instance
        control = &DGControl::getInstance();
        control->init();
        
        // Check if we must enable vertical sync
        if (config->verticalSync) {
            // Get the available extensions in the card
            char* extensions = (char*)glGetString(GL_EXTENSIONS);
            
            // Is vertical sync possible?
            if (strstr(extensions,"WGL_EXT_swap_control")) {
                // If so get the address of the functions we defined before
                wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)
                wglGetProcAddress("wglSwapIntervalEXT");
                wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)
                wglGetProcAddress("wglGetSwapIntervalEXT");
                
                // Go ahead and enable
                wglSwapIntervalEXT(1);
            }
            
            log->error(DGModSystem, "%s", DGMsg240003);
        }
        
        if (config->fullScreen)
            toggleFullScreen();
        
        ShowCursor(FALSE);        
        
        _isInitialized = true;
        log->trace(DGModSystem, "%s", DGMsg040001);
    }
    else log->warning(DGModSystem, "%s", DGMsg140002);
}

void DGSystem::run() {
    DWORD dwControlThreadId;
	HANDLE hThread;
	MSG uMsg;
	
    // Create the thread to update the controller module
	if (!(hThread = CreateThread(NULL, 0, _systemThread, NULL, 0, &dwControlThreadId)))
		log->error(DGModSystem, "%s", DGMsg240004);
	
    // Now launch the main loop
	memset(&uMsg,0,sizeof(uMsg));
    while (uMsg.message != WM_QUIT) {
        if (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&uMsg);
            DispatchMessage(&uMsg);
        }
    }
}

void DGSystem::setTitle(const char* title) {
    // Implement
}

// TODO: We must delete the thread here too
void DGSystem::terminate() {
	if (g_hRC != NULL) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(g_hRC);
        g_hRC = NULL;
    }
	
    if (g_hDC != NULL) {
        ReleaseDC(g_hWnd, g_hDC);
        g_hDC = NULL;
    }
	
	UnregisterClass(L"DG_WINDOWS_CLASS", NULL);
	ShowCursor(TRUE);
	
	PostQuitMessage(0);
}

// TODO: We should try and get the best possible resolution here
void DGSystem::toggleFullScreen() {
    config->fullScreen = !config->fullScreen;
    
    if (config->fullScreen) {
        // Enter fullscreen        
        DEVMODE fullscreenSettings;
        
        EnumDisplaySettings(NULL, 0, &fullscreenSettings);
        fullscreenSettings.dmPelsWidth        = config->displayWidth;
        fullscreenSettings.dmPelsHeight       = config->displayHeight;
        fullscreenSettings.dmBitsPerPel       = config->displayDepth;
        fullscreenSettings.dmDisplayFrequency = 60; // For compatibility purposes, we always force 60 fps here
        fullscreenSettings.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
        
        SetWindowLongPtr(g_hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
        SetWindowLongPtr(g_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, config->displayWidth, config->displayHeight, SWP_SHOWWINDOW);
        if (ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
            ShowWindow(g_hWnd, SW_MAXIMIZE);
        }
        else log->error(DGModSystem, "%s", DGMsg240005);
    }
    else {
        // Exit fullscreen
        SetWindowLongPtr(g_hWnd, GWL_EXSTYLE, WS_EX_LEFT);
        SetWindowLongPtr(g_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        if (ChangeDisplaySettings(NULL, CDS_RESET) == DISP_CHANGE_SUCCESSFUL) {
            SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, config->displayWidth, config->displayHeight, SWP_SHOWWINDOW);
            ShowWindow(g_hWnd, SW_RESTORE);
        }
        else log->error(DGModSystem, "%s", DGMsg240006);        
    }
}

void DGSystem::update() {
	// Update the controller
	control->update();

    // Now swap the OpenGL buffers
    SwapBuffers(g_hDC);
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

DWORD WINAPI _systemThread(LPVOID lpParam) {
	while (true) {
		DGSystem::getInstance().update();
		Sleep(1);
	}
	
	return 0;
}

// This function processes the main loop
LRESULT CALLBACK _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {	
    switch(msg) {
		case WM_SIZE:
			DGConfig::getInstance().displayWidth = LOWORD(lParam);
			DGConfig::getInstance().displayHeight = HIWORD(lParam);
			break;
		case WM_MOUSEMOVE:
            DGControl::getInstance().processMouse(LOWORD(lParam), HIWORD(lParam), false);
			break;
		case WM_LBUTTONDOWN:
            // We ignore this message
			break;
		case WM_LBUTTONUP:
			DGControl::getInstance().processMouse(LOWORD(lParam), HIWORD(lParam), true);
			break;
		case WM_KEYDOWN:
            // A different switch to handle keystrokes
            switch(wParam) {
				case VK_F1:
				case VK_F2:
				case VK_F3:	
				case VK_F4:
				case VK_F5:
				case VK_F6:
				case VK_F7:
				case VK_F8:
				case VK_F9:
				case VK_F10:
				case VK_F11:
				case VK_F12:
					DGControl::getInstance().processKey(wParam, false);
					break;
				case VK_SHIFT:
					// Ignored when pressed alone
					break;
				case VK_ESCAPE:
				case VK_RETURN:
				case VK_BACK:	
				case VK_OEM_3:
				default:
                    WORD ch;
                    BYTE kbs[256];
                    
					GetKeyboardState(kbs);
					ToAscii(wParam, MapVirtualKey(wParam, 0), kbs, &ch, 0);
					DGControl::getInstance().processKey(ch, false);
					break;
			}			
			break;
        case WM_CLOSE:
        case WM_DESTROY:
		case WM_QUIT:
            // Simulate the ESC key
            DGControl::getInstance().processKey(DGKeyEsc, false);
			break;			
        default:
            // Any other messages are passed to the default window process
            return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	
	return 0;
}
