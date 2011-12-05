/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGAudio.h"
#include "DGCommonGL.h"
#include "DGConfig.h"
#include "DGConsole.h"
#include "DGControl.h"
#include "DGLog.h"
#include "DGScript.h"
#include "DGSystem.h"
#include "DGVideo.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#ifndef DG_USE_SDL


HWND g_hWnd = NULL;
HDC g_hDC = NULL;
HGLRC g_hRC = NULL;

LRESULT CALLBACK _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CRITICAL_SECTION cs;

// ------------------------------------ Vsync functions --------------------------------------//

//function pointer typdefs
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);

//declare functions
PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;

//init VSync func
int InitVSync()
{
	//get extensions of graphics card
	char* extensions = (char*)glGetString(GL_EXTENSIONS);
	
	//is WGL_EXT_swap_control in the string? VSync switch possible?
	if (strstr(extensions,"WGL_EXT_swap_control"))
	{
		//get address's of both functions and save them
		wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)
		wglGetProcAddress("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)
		wglGetProcAddress("wglGetSwapIntervalEXT");
		
		return 1;
	}
	
	return 0;
}

int VSyncEnabled()
{
	//if interval is positif, it is not 0 so enabled ;)
	return (wglGetSwapIntervalEXT() > 0);
}

void SetVSyncState(int enable)
{
    if (enable)
		wglSwapIntervalEXT(1); //set interval to 1 -> enable
    else
		wglSwapIntervalEXT(0); //disable
}

// ------------------------------------ Vsync functions --------------------------------------//

BOOL enterFullscreen(HWND hwnd, int fullscreenWidth, int fullscreenHeight, int colourBits, int refreshRate) {
    DEVMODE fullscreenSettings;
    BOOL isChangeSuccessful;
	
    EnumDisplaySettings(NULL, 0, &fullscreenSettings);
    fullscreenSettings.dmPelsWidth        = fullscreenWidth;
    fullscreenSettings.dmPelsHeight       = fullscreenHeight;
    fullscreenSettings.dmBitsPerPel       = colourBits;
    fullscreenSettings.dmDisplayFrequency = refreshRate;
    fullscreenSettings.dmFields           = DM_PELSWIDTH |
	DM_PELSHEIGHT |
	DM_BITSPERPEL |
	DM_DISPLAYFREQUENCY;
	
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
    SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, fullscreenWidth, fullscreenHeight, SWP_SHOWWINDOW);
    isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
    ShowWindow(hwnd, SW_MAXIMIZE);
	
    return isChangeSuccessful;
}

BOOL exitFullscreen(HWND hwnd, int windowX, int windowY, int windowedWidth, int windowedHeight, int windowedPaddingX, int windowedPaddingY) {
    BOOL isChangeSuccessful;
	
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_LEFT);
    SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    isChangeSuccessful = ChangeDisplaySettings(NULL, CDS_RESET) == DISP_CHANGE_SUCCESSFUL;
    SetWindowPos(hwnd, HWND_NOTOPMOST, windowX, windowY, windowedWidth + windowedPaddingX, windowedHeight + windowedPaddingY, SWP_SHOWWINDOW);
    ShowWindow(hwnd, SW_RESTORE);
	
    return isChangeSuccessful;
}

DWORD WINAPI AudioThread(LPVOID lpParam) {
	while (DGAudioLoop()) {
		Sleep(1);
	}
	
	return 0;
}

DWORD WINAPI VideoThread(LPVOID lpParam) {
	while (DGVideoLoop()) {
		Sleep(1);
	}
	
	return 0;
}

void DGSystemFindPaths() {
	// Implement
}

void DGSystemInitialize() {
	WNDCLASSEX winClass;
	GLuint PixelFormat;
	PIXELFORMATDESCRIPTOR pfd;
	WCHAR title[DG_MAX_PATHLEN];
	MultiByteToWideChar(0, 0, DGConfig.script, DG_MAX_PATHLEN, title, DG_MAX_PATHLEN);
	
	DGLogTrace(DG_MOD_SYSTEM, "Initializing system interface...");
	
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
	
#ifndef DG_TOOL	
    g_hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"DG_WINDOWS_CLASS", 
							title,
							WS_OVERLAPPEDWINDOW | WS_VISIBLE,
							0, 0, DGConfig.display_width, DGConfig.display_height, NULL, NULL, NULL, NULL );
	
    if (g_hWnd == NULL)
        return;
	
    ShowWindow(g_hWnd, SW_SHOWNORMAL);
    UpdateWindow(g_hWnd);
#endif

    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	
    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW |PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 16;
    pfd.cDepthBits = 16;
    
    g_hDC = GetDC( g_hWnd );
    PixelFormat = ChoosePixelFormat( g_hDC, &pfd );
    SetPixelFormat( g_hDC, PixelFormat, &pfd);
    g_hRC = wglCreateContext( g_hDC );
    wglMakeCurrent( g_hDC, g_hRC );
	
	if (DGConfig.vertical_sync) {
		if (InitVSync())
			SetVSyncState(1);
	}
	
	if (DGConfig.fullscreen)
		enterFullscreen(g_hWnd, DGConfig.display_width, DGConfig.display_height, 24, 60);
	
    InitializeCriticalSection(&cs);
	
	//ShowCursor(FALSE);
}

void DGSystemLock() {
	EnterCriticalSection(&cs);
}

void DGSystemRun() {
	DWORD  dwThreadId;
	HANDLE hThread;
	MSG uMsg;
	
	hThread = CreateThread(NULL, 0, AudioThread, NULL, 0, &dwThreadId);
	
	if (!hThread) {
		DGLogError(DG_MOD_SYSTEM, "Could not create audio thread");
	}

	hThread = CreateThread(NULL, 0, VideoThread, NULL, 0, &dwThreadId);
	
	if (!hThread) {
		DGLogError(DG_MOD_SYSTEM, "Could not create video thread");
	}
	
	memset(&uMsg,0,sizeof(uMsg));
    while( uMsg.message != WM_QUIT )
    {
        if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &uMsg );
            DispatchMessage( &uMsg );
        }
        else {
			DGScriptLoop();
		}
    }
}

void DGSystemTerminate() {
	if( g_hRC != NULL )
    {
        wglMakeCurrent( NULL, NULL );
        wglDeleteContext( g_hRC );
        g_hRC = NULL;
    }
	
    if( g_hDC != NULL )
    {
        ReleaseDC( g_hWnd, g_hDC );
        g_hDC = NULL;
    }
	
    DeleteCriticalSection(&cs);
	
	UnregisterClass(L"DG_WINDOWS_CLASS", NULL);
	ShowCursor(TRUE);
	
	PostQuitMessage(0);
}

void DGSystemSetTitle(const char* title) {
	// Implement
}

void DGSystemToggleFullscreen() {
	// Implement
}

void DGSystemUpdate() {
	SwapBuffers(g_hDC);
}

void DGSystemUnlock() {
	LeaveCriticalSection(&cs);
}

LRESULT CALLBACK _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	WORD ch;
	BYTE kbs[256];
	char key[2];
	
	int width, height;
    static POINT mouse;
	
    switch( msg )
    {
		case WM_SIZE:
			width = LOWORD(lParam); 
            height = HIWORD(lParam);
			
			DGConfig.display_width = width;
			DGConfig.display_height = height;
			break;
		case WM_MOUSEMOVE:
			mouse.x = LOWORD (lParam);
            mouse.y = HIWORD (lParam);
			DGControlProcessMouse((short)mouse.x, DGConfig.display_height - (short)mouse.y, DG_NO);
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			mouse.x = LOWORD (lParam);
            mouse.y = HIWORD (lParam);
			DGControlProcessMouse((short)mouse.x, DGConfig.display_height - (short)mouse.y, DG_YES);
			break;
		case WM_KEYDOWN:
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
					DGControlProcessKey(wParam);
					break;
				case VK_SHIFT:
					// Ignore when alone
					break;
				case VK_ESCAPE:
				case VK_RETURN:
				case VK_BACK:	
				case VK_OEM_3:
				default:
					GetKeyboardState(kbs);
					ToAscii(wParam, MapVirtualKey(wParam, 0), kbs, &ch, 0);
					sprintf(key, "%c", ch);
					DGControlProcessCharKey(key, DG_NO);
					break;
			}			
			break;
		/*case WM_CHAR:
			sprintf(key, "%c", wParam);
			dg_control_process_key(key);
			break;*/
        case WM_CLOSE:
        case WM_DESTROY:
		case WM_QUIT:
            DGControlProcessKey(DG_KEY_ESC);
			break;			
        default:
            return DefWindowProc( hWnd, msg, wParam, lParam );
			break;
	}
	
	return 0;
}

#endif
