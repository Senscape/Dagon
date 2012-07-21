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

// Tell the Windows subsystem that we shouldn't create a console
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

// These are private in order to keep a clean and portable
// header

HWND g_hWnd = NULL;
HDC g_hDC = NULL;
HGLRC g_hRC = NULL;

HANDLE hAudioThread;
HANDLE hProfilerThread;
HANDLE hSystemThread;
HANDLE hTimerThread;
HANDLE hVideoThread;

CRITICAL_SECTION csAudioThread;
CRITICAL_SECTION csSystemThread;
CRITICAL_SECTION csTimerThread;
CRITICAL_SECTION csVideoThread;

DWORD WINAPI _audioThread(LPVOID lpParam);
DWORD WINAPI _profilerThread(LPVOID lpParam);
DWORD WINAPI _systemThread(LPVOID lpParam);
DWORD WINAPI _timerThread(LPVOID lpParam);
DWORD WINAPI _videoThread(LPVOID lpParam);
LRESULT CALLBACK _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Get pointers to functions for setting vertical sync
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);

PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;

BYTE keyboardState[256];
int previousWidth;
int previousHeight;

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

void DGSystem::createThreads() {
	InitializeCriticalSection(&csAudioThread);
	hAudioThread = CreateThread(NULL, 0, _audioThread, NULL, 0, NULL);

	InitializeCriticalSection(&csTimerThread);
	hTimerThread = CreateThread(NULL, 0, _timerThread, NULL, 0, NULL);

	InitializeCriticalSection(&csVideoThread);
	hVideoThread = CreateThread(NULL, 0, _videoThread, NULL, 0, NULL);

	if (config->debugMode) {
		hProfilerThread = CreateThread(NULL, 0, _profilerThread, NULL, 0, NULL);
	}

	_areThreadsActive = true;
}

void DGSystem::destroyThreads() {
	if (hAudioThread != NULL) {
		EnterCriticalSection(&csAudioThread);
		DGAudioManager::getInstance().terminate();
		LeaveCriticalSection(&csAudioThread);
		WaitForSingleObject(hAudioThread, INFINITE);
		DeleteCriticalSection(&csAudioThread);
	}

	if (hTimerThread != NULL) {
		EnterCriticalSection(&csTimerThread);
		DGTimerManager::getInstance().terminate();
		LeaveCriticalSection(&csTimerThread);
		WaitForSingleObject(hTimerThread, INFINITE);
		DeleteCriticalSection(&csTimerThread);
	}

	if (hVideoThread != NULL) {
		EnterCriticalSection(&csVideoThread);
		DGVideoManager::getInstance().terminate();
		LeaveCriticalSection(&csVideoThread);
		WaitForSingleObject(hVideoThread, INFINITE);
		DeleteCriticalSection(&csVideoThread);
	}

	_areThreadsActive = false;
}

void DGSystem::findPaths(int argc, char* argv[]) {
    // Implement
}

void DGSystem::init() {
    if (!_isInitialized) {
        log->trace(DGModSystem, "========================================");
        log->trace(DGModSystem, "%s", DGMsg040000); 

		InitializeCriticalSection(&csSystemThread);

        // Prepare the string to set the window title
        WCHAR title[DGMaxPathLength];
        MultiByteToWideChar(0, 0, config->script(), DGMaxFileLength, title, DGMaxFileLength);
        
        WNDCLASSEX winClass;        
        winClass.lpszClassName = L"DG_WINDOWS_CLASS";
        winClass.cbSize        = sizeof(WNDCLASSEX);
        winClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        winClass.lpfnWndProc   = _WindowProc;
        winClass.hInstance     = NULL;
		winClass.hIcon         = LoadIcon(NULL, MAKEINTRESOURCE(100));
		winClass.hIconSm       = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(100), IMAGE_ICON, 16, 16, 0);
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

		// FIXME: This doesn't work if the CTRL key is pressed when launching
		GetKeyboardState(keyboardState);
        
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
            else log->error(DGModSystem, "%s", DGMsg240003);
        }
        
        ShowCursor(FALSE);

        _isInitialized = true;
        log->trace(DGModSystem, "%s", DGMsg040001);
    }
    else log->warning(DGModSystem, "%s", DGMsg140002);
}

void DGSystem::resumeThread(int threadID){
    if (_areThreadsActive) {
        switch (threadID) {
            case DGAudioThread:
                LeaveCriticalSection(&csAudioThread);
                break;
            case DGTimerThread:
                LeaveCriticalSection(&csTimerThread);
                break;
            case DGVideoThread:
                LeaveCriticalSection(&csVideoThread);
                break;                
        }
    }
}

void DGSystem::run() {
	wglMakeCurrent(NULL, NULL);

    // Create the thread to update the controller module
	if (!(hSystemThread = CreateThread(NULL, 0, _systemThread, NULL, 0, NULL)))
		log->error(DGModSystem, "%s", DGMsg240004);

	if (config->fullScreen)
        toggleFullScreen();

    // Now launch the main loop
	MSG uMsg;
	memset(&uMsg,0,sizeof(uMsg));

	_isRunning = true;
    while (_isRunning) {
        if (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&uMsg);
            DispatchMessage(&uMsg);
        }
    }

	WaitForSingleObject(hSystemThread, INFINITE);
	if (config->debugMode) {
		WaitForSingleObject(hProfilerThread, INFINITE);
	}

	DeleteCriticalSection(&csSystemThread);
}

void DGSystem::setTitle(const char* title) {
	/*WCHAR str[DGMaxLogLength];
	MultiByteToWideChar(0,0, title, DGMaxLogLength, str, DGMaxLogLength);
    SetWindowText(g_hWnd, str);*/
}

void DGSystem::suspendThread(int threadID){
    if (_areThreadsActive) {
        switch (threadID) {
            case DGAudioThread:
                EnterCriticalSection(&csAudioThread);
                break;
            case DGTimerThread:
                EnterCriticalSection(&csTimerThread);
                break;
            case DGVideoThread:
                EnterCriticalSection(&csVideoThread);
                break;                
        }
    }
}

// TODO: We must delete the thread here too
void DGSystem::terminate() {
	if (_areThreadsActive)
		this->destroyThreads();

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
	
	_isRunning = false;
}

// TODO: We should try and get the best possible resolution here
void DGSystem::toggleFullScreen() { 
    if (config->fullScreen) {
		// Enter fullscreen

		previousWidth = config->displayWidth;
		previousHeight = config->displayHeight;
       
		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		// The top left corner will have coordinates (0,0)
		// and the bottom right corner will have coordinates
		// (horizontal, vertical)

        DEVMODE fullscreenSettings;
        
        EnumDisplaySettings(NULL, 0, &fullscreenSettings);
        fullscreenSettings.dmPelsWidth        = desktop.right;
        fullscreenSettings.dmPelsHeight       = desktop.bottom;
        fullscreenSettings.dmBitsPerPel       = config->displayDepth;
        fullscreenSettings.dmDisplayFrequency = config->framerate;
        fullscreenSettings.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
        
        SetWindowLongPtr(g_hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
        SetWindowLongPtr(g_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, desktop.right, desktop.bottom, SWP_SHOWWINDOW);
        if (ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
            ShowWindow(g_hWnd, SW_MAXIMIZE);
			control->reshape(desktop.right, desktop.bottom);
        }
        else log->error(DGModSystem, "%s", DGMsg240005);
    }
    else {
        // Exit fullscreen
		config->displayWidth = previousWidth;
		config->displayHeight = previousHeight;

        SetWindowLongPtr(g_hWnd, GWL_EXSTYLE, WS_EX_LEFT);
        SetWindowLongPtr(g_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        if (ChangeDisplaySettings(NULL, CDS_RESET) == DISP_CHANGE_SUCCESSFUL) {
            SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, config->displayWidth, config->displayHeight, SWP_SHOWWINDOW);
            ShowWindow(g_hWnd, SW_RESTORE);
			control->reshape(config->displayWidth, config->displayHeight);
        }
        else log->error(DGModSystem, "%s", DGMsg240006);        
    }
}

void DGSystem::update() {
    // Now swap the OpenGL buffers
    SwapBuffers(g_hDC);
}

time_t DGSystem::wallTime() {
	FILETIME ft;
	LARGE_INTEGER li;
	time_t ret;

	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	ret = li.QuadPart;
	ret -= 116444736000000000LL;
	ret /= 10000;

	return ret;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

DWORD WINAPI _audioThread(LPVOID lpParam) {
	DWORD dwPause = 10;
	bool isRunning = true;

	while (isRunning) {
		EnterCriticalSection(&csAudioThread);
		isRunning = DGAudioManager::getInstance().update();
		LeaveCriticalSection(&csAudioThread);
		Sleep(dwPause);
	}
	
	return 0;
}

DWORD WINAPI _profilerThread(LPVOID lpParam) {
	DWORD dwPause = 1000;
	bool isRunning = true;

	while (isRunning) {
		EnterCriticalSection(&csSystemThread);
		isRunning = DGControl::getInstance().profiler();
		LeaveCriticalSection(&csSystemThread);
		Sleep(dwPause);
	}
	
	return 0;
}

DWORD WINAPI _systemThread(LPVOID lpParam) {
	DWORD dwPause = (1.0f / DGConfig::getInstance().framerate) * 1000;
	bool isRunning = true;

	while (isRunning) {
		EnterCriticalSection(&csSystemThread);
		wglMakeCurrent(g_hDC, g_hRC);
		isRunning = DGControl::getInstance().update();
		wglMakeCurrent(NULL, NULL);
		LeaveCriticalSection(&csSystemThread);
		Sleep(dwPause);
	}
	
	return 0;
}

DWORD WINAPI _timerThread(LPVOID lpParam) {
	DWORD dwPause = 100;
	bool isRunning = true;

	while (isRunning) {
		EnterCriticalSection(&csTimerThread);
		isRunning = DGTimerManager::getInstance().update();
		LeaveCriticalSection(&csTimerThread);
		Sleep(dwPause);
	}
	
	return 0;
}

DWORD WINAPI _videoThread(LPVOID lpParam) {
	DWORD dwPause = 10;
	bool isRunning = true;

	while (isRunning) {
		EnterCriticalSection(&csVideoThread);
		isRunning = DGVideoManager::getInstance().update();
		LeaveCriticalSection(&csVideoThread);
		Sleep(dwPause);
	}
	
	return 0;
}

// This function processes the main loop
LRESULT CALLBACK _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static bool isDragging = false;

    switch(msg) {
		case WM_ERASEBKGND:
			break;
		case WM_SIZE:
			EnterCriticalSection(&csSystemThread);
			wglMakeCurrent(g_hDC, g_hRC);
			DGControl::getInstance().reshape(LOWORD(lParam), HIWORD(lParam));
			wglMakeCurrent(NULL, NULL);
			LeaveCriticalSection(&csSystemThread);
			break;
		case WM_MOUSEMOVE:
			EnterCriticalSection(&csSystemThread);
			wglMakeCurrent(g_hDC, g_hRC);
			if (isDragging)
				DGControl::getInstance().processMouse(LOWORD(lParam), HIWORD(lParam), DGMouseEventDrag);
			else
				DGControl::getInstance().processMouse(LOWORD(lParam), HIWORD(lParam), DGMouseEventMove);
			wglMakeCurrent(NULL, NULL);
			LeaveCriticalSection(&csSystemThread);

			if (GetCapture() != g_hWnd) {
				SetCapture(g_hWnd);
			}
			else {
				RECT rect;
				GetWindowRect(g_hWnd, &rect);

				POINT pt = { LOWORD(lParam), HIWORD(lParam) };
				ClientToScreen(g_hWnd, &pt);

				if (!PtInRect(&rect, pt)) {
					EnterCriticalSection(&csSystemThread);
					wglMakeCurrent(g_hDC, g_hRC);
					DGControl::getInstance().processMouse(DGConfig::getInstance().displayWidth / 2, 
					DGConfig::getInstance().displayHeight / 2, DGMouseEventMove);
					wglMakeCurrent(NULL, NULL);
					LeaveCriticalSection(&csSystemThread);

					ReleaseCapture();
				}
			}
			break;
		case WM_LBUTTONDOWN:
			EnterCriticalSection(&csSystemThread);
			wglMakeCurrent(g_hDC, g_hRC);
			DGControl::getInstance().processMouse(LOWORD(lParam), HIWORD(lParam), DGMouseEventDown);
			wglMakeCurrent(NULL, NULL);
			LeaveCriticalSection(&csSystemThread);
			isDragging = true;
			break;
		case WM_LBUTTONUP:
			EnterCriticalSection(&csSystemThread);
			wglMakeCurrent(g_hDC, g_hRC);
			DGControl::getInstance().processMouse(LOWORD(lParam), HIWORD(lParam), DGMouseEventUp);
			wglMakeCurrent(NULL, NULL);
			LeaveCriticalSection(&csSystemThread);
			isDragging = false;
			break;
		case WM_KEYDOWN:
			EnterCriticalSection(&csSystemThread);
			wglMakeCurrent(g_hDC, g_hRC);
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
					DGControl::getInstance().processFunctionKey(wParam);
					break;
				case VK_SHIFT:
					// Ignored when pressed alone
					break;
				case VK_ESCAPE:
				case VK_TAB:
				case VK_SPACE:
				case VK_RETURN:
				case VK_BACK:	
					DGControl::getInstance().processKey(wParam, false);
					break;
				case VK_OEM_3: // Open console
					DGControl::getInstance().processKey(DGKeyTab, false);
					break;
				default:
					WCHAR chars[2];
					ToUnicode(wParam, MapVirtualKey(wParam, 0), keyboardState, chars, 2, 0);
					if (GetKeyState(VK_CONTROL) < 0) {
						DGControl::getInstance().processKey(chars[0], true);
					}
					else DGControl::getInstance().processKey(chars[0], false);
					break;
			}
			wglMakeCurrent(NULL, NULL);
			LeaveCriticalSection(&csSystemThread);
			break;
        case WM_CLOSE:
            // Simulate the ESC key
			EnterCriticalSection(&csSystemThread);
			wglMakeCurrent(g_hDC, g_hRC);
            DGControl::getInstance().processKey(DGKeyEsc, false);
			wglMakeCurrent(NULL, NULL);
			LeaveCriticalSection(&csSystemThread);
			break;
        case WM_DESTROY:
		case WM_QUIT:			
        default:
            // Any other messages are passed to the default window process
            return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	
	return 0;
}
