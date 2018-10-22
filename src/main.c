#include <Windows.h>
#include <string.h>

#define IDM_APP_EXIT        40000

#define goto_if_valid(p)    do { if (p != NULL) goto *p; } while (0)

static const TCHAR MainWindowName[] = TEXT("Mandelbrot Set");

static void MessageBox_Error(const TCHAR *errStr);
static HMENU MainWindowMenu(void);
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

static int is_bounded(_Complex double value);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    MSG msg;
    int msgres = 0;
    void *next = NULL;
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
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = MainWindowName;

    __auto_type RegisterClassResult = RegisterClass(&wndclass);

    next = (RegisterClassResult == 0) ? &&RegisterClassError : &&RegisterClassOk;
    goto *next;

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

    next = (hWindow == NULL) ? &&CreateWindowError : &&CreateWindowOk;
    goto *next;

CreateWindowOk:
    ShowWindow(hWindow, iCmdShow);

    __auto_type UpdateWindowResult = UpdateWindow(hWindow);

    next = (UpdateWindowResult == FALSE) ? &&UpdateWindowError : &&UpdateWindowOk;
    goto *next;

UpdateWindowOk:
    msgres = GetMessage(&msg, NULL, 0, 0);

loop_start:
    next = ((msgres != 0) && (msgres != -1)) ? NULL : &&loop_end;
    goto_if_valid(next);

    TranslateMessage(&msg);
    DispatchMessage(&msg);

    msgres = GetMessage(&msg, NULL, 0, 0);
    goto loop_start;

loop_end:

    next = (msgres == -1) ? &&GetMessageError : NULL;
    goto_if_valid(next);

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
    AppendMenu(hCtrlMenu, MF_STRING, IDM_APP_EXIT, TEXT("&Exit"));

    __auto_type hMenu = CreateMenu();
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hCtrlMenu, TEXT("&File"));

    return hMenu;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static const double Limit_Re[2] = { -2.0, +1.0 };
    static const double Limit_Im[2] = { -1.0, +1.0 };

    static double step_x = 0.0;
    static double step_y = 0.0;

    static int max_x = 0;
    static int max_y = 0;
    int x = 0;
    int y = 0;

    HDC hdc = NULL;
    PAINTSTRUCT ps;
    void *next = NULL;

    next = (message == WM_SIZE) ? &&WM_SIZE_Handler : NULL;
    goto_if_valid(next);

    next = (message == WM_CREATE) ? &&WM_CREATE_Handler : NULL;
    goto_if_valid(next);

    next = (message == WM_COMMAND) ? &&WM_COMMAND_Handler : NULL;
    goto_if_valid(next);

    next = (message == WM_DESTROY) ? &&WM_DESTROY_Handler : NULL;
    goto_if_valid(next);

    next = (message == WM_PAINT) ? &&WM_PAINT_Handler : NULL;
    goto_if_valid(next);

    return DefWindowProc(hwnd, message, wParam, lParam);

WM_CREATE_Handler:
    return 0;

WM_DESTROY_Handler:
    PostQuitMessage(0);
    return 0;

WM_PAINT_Handler:
    hdc = BeginPaint(hwnd, &ps);

    const COLORREF rgb = RGB(0, 0, 0);
    _Complex c;
    __real__ c = Limit_Re[0];
    __imag__ c = Limit_Im[0];

    x = 0;
loop_x_start:

    y = 0;
loop_y_start:

    __imag__ c += step_y;

    next = (0 != is_bounded(c)) ? &&is_bounded_case : &&is_unbounded_case;
    goto *next;

is_bounded_case:
    SetPixel(hdc, x, y, rgb);

is_unbounded_case:
    SetPixel(hdc, x, y, rgb);

    ++y;
    next = (y < max_y) ? &&loop_y_start : &&loop_y_end;
    goto *next;
loop_y_end:

    __real__ c += step_x;
    ++x;
    next = (x < max_x) ? &&loop_x_start : &&loop_x_end;
    goto *next;
loop_x_end:

    EndPaint(hwnd, &ps);
    return 0;

WM_COMMAND_Handler:
    next = (IDM_APP_EXIT == LOWORD(wParam)) ? &&AppExitCommand : NULL;
    goto_if_valid(next);
    return 0;

WM_SIZE_Handler:

    max_x = LOWORD(lParam);
    step_x = (Limit_Re[1] - Limit_Re[0]) / (double)max_x;

    max_y = HIWORD(lParam);
    step_y = (Limit_Im[1] - Limit_Im[0]) / (double)max_y;

    return 0;

AppExitCommand:
    PostMessage(hwnd, WM_CLOSE, 0, 0);
    return 0;
}

static int is_bounded(_Complex double value)
{
    _Complex double z = 0.0 + 0.0j;
    double result = 0.0;
    double x = 0.0;
    double y = 0.0;
    void *next = NULL;
    int count = 0;

count_start:
    x = (__real__ z * __real__ z) - (__imag__ z * __imag__ z) + __real__ value;

    y = (2.0 * __real__ z * __imag__ z) + __imag__ value;

    result = (x * x) + (y * y);

    next = (result > 4.0) ? &&not_bounded : &&bounded;
    goto *next;

bounded:
    __real__ z = x;
    __imag__ z = y;

    ++count;
    next = (count < 100) ? &&count_start : &&count_end;
    goto *next;

count_end:
    return 1;

not_bounded:
    return 0;
}
