#include <Windows.h>
#include <string.h>

#define IDM_APP_EXIT        40000

#define goto_if_valid(p)    do { if (p != NULL) goto *p; } while (0)

static const TCHAR MainWindowName[] = TEXT("Mandelbrot Set");

static void MessageBox_Error(const TCHAR *errStr);
static HMENU MainWindowMenu(void);
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    MSG msg;
    int msgres = 0;
    void *loc = NULL;
    HWND hWindow = NULL;

    hPrevInstance = hPrevInstance;
    szCmdLine = szCmdLine;

    WNDCLASS wndclass;
    memset(&wndclass, 0, sizeof(wndclass));

    wndclass.style         = (CS_HREDRAW | CS_VREDRAW);
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = MainWindowName;

    __auto_type RegisterClassResult = RegisterClass(&wndclass);

    loc = (RegisterClassResult == 0) ? &&RegisterClassError : &&RegisterClassOk;
    goto *loc;

RegisterClassOk:
    hWindow = CreateWindow(
            MainWindowName,
            MainWindowName,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            NULL,
            MainWindowMenu(),
            hInstance,
            NULL);

    loc = (hWindow == NULL) ? &&CreateWindowError : &&CreateWindowOk;
    goto *loc;

CreateWindowOk:
    ShowWindow(hWindow, iCmdShow);

    __auto_type UpdateWindowResult = UpdateWindow(hWindow);

    loc = (UpdateWindowResult == FALSE) ? &&UpdateWindowError : &&UpdateWindowOk;
    goto *loc;

UpdateWindowOk:
    msgres = GetMessage(&msg, NULL, 0, 0);

loop_start:
    loc = ((msgres != 0) && (msgres != -1)) ? NULL : &&loop_end;
    goto_if_valid(loc);

    TranslateMessage(&msg);
    DispatchMessage(&msg);

    msgres = GetMessage(&msg, NULL, 0, 0);
    goto loop_start;

loop_end:

    loc = (msgres == -1) ? &&GetMessageError : NULL;
    goto_if_valid(loc);

    return 0;

UpdateWindowError:
    MessageBox_Error(TEXT("Error in UpdateWindow"));
    return 0;

CreateWindowError:
    MessageBox_Error(TEXT("Error in CreateWindow"));
    return 0;

RegisterClassError:
    MessageBox_Error(TEXT("Error in RegisterClass"));
    return 0;

GetMessageError:
    MessageBox_Error(TEXT("Error in GetMessage"));
    return 0;
}

static void MessageBox_Error(const TCHAR *errStr)
{
    (void)MessageBox(NULL, errStr, MainWindowName, MB_ICONERROR);
}

static HMENU MainWindowMenu(void)
{
    __auto_type hCtrlMenu = CreateMenu();
    {
        AppendMenu(hCtrlMenu, MF_STRING, IDM_APP_EXIT, TEXT("&Exit"));
    }

    __auto_type hMenu = CreateMenu();
    {
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hCtrlMenu, TEXT("&File"));
    }

    return hMenu;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    void *handler = NULL;

    handler = (message == WM_SIZE) ? &&WM_SIZE_Handler : NULL;
    goto_if_valid(handler);

    handler = (message == WM_CREATE) ? &&WM_CREATE_Handler : NULL;
    goto_if_valid(handler);

    handler = (message == WM_COMMAND) ? &&WM_COMMAND_Handler : NULL;
    goto_if_valid(handler);

    handler = (message == WM_DESTROY) ? &&WM_DESTROY_Handler : NULL;
    goto_if_valid(handler);

    handler = (message == WM_PAINT) ? &&WM_PAINT_Handler : NULL;
    goto_if_valid(handler);

    return DefWindowProc(hwnd, message, wParam, lParam);

WM_CREATE_Handler:
    return 0;

WM_DESTROY_Handler:
    PostQuitMessage(0);
    return 0;

WM_PAINT_Handler:
    (void)BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);
    return 0;

WM_COMMAND_Handler:
    handler = (IDM_APP_EXIT == LOWORD(wParam)) ? &&AppExitCommand : NULL;
    goto_if_valid(handler);
    return 0;

WM_SIZE_Handler:
    return 0;

AppExitCommand:
    PostMessage(hwnd, WM_CLOSE, 0, 0);
    return 0;
}
