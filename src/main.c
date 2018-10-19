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
    hPrevInstance = hPrevInstance;
    szCmdLine = szCmdLine;
    iCmdShow = iCmdShow;

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

    if (0 == RegisterClassResult)
    {
        MessageBox_Error(TEXT("Error in RegisterClass"));
        return 0;
    }

    __auto_type hWindow = CreateWindow(
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

    if (hWindow == NULL)
    {
        MessageBox_Error(TEXT("Error in CreateWindow"));
        return 0;
    }

    ShowWindow(hWindow, iCmdShow);

    __auto_type UpdateWindowResult = UpdateWindow(hWindow);

    if (UpdateWindowResult == FALSE)
    {
        MessageBox_Error(TEXT("Error in UpdateWindow"));
        return 0;
    }

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
    return 0;

WM_SIZE_Handler:
    return 0;
}
