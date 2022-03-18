#include "main_window.h"

HHOOK keyHook;

BOOL main_window::create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
    WNDCLASS wc = {0};

    wc.lpfnWndProc = main_window::WindowProc;
    wc.hInstance = h_instance;
    wc.lpszClassName = class_name();

    RegisterClass(&wc);

    RECT rect_size;

    rect_size.left = 0;
    rect_size.top = 0;
    rect_size.right = nWidth;
    rect_size.bottom = nHeight;

    AdjustWindowRect(&rect_size, dwStyle, hMenu != nullptr);

    h_main_window = CreateWindowEx(
        dwExStyle, class_name(), lpWindowName, dwStyle, x, y,
        rect_size.right - rect_size.left, rect_size.bottom - rect_size.top, hWndParent, hMenu, h_instance, this);

    keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, main_window::keyProc, nullptr, 0);

    return (h_main_window ? TRUE : FALSE);
}

void main_window::setup_ui()
{
    h_start_button = CreateWindow(
        L"BUTTON",                             // Predefined class; Unicode assumed
        L"START",                              // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Styles
        btn_x,                                 // x position
        btn_y,                                 // y position
        btn_w,                                 // Button width
        btn_h,                                 // Button height
        h_main_window,                         // Parent window
        reinterpret_cast<HMENU>(start_button),
        h_instance,
        NULL);
}

LRESULT main_window::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        setup_ui();
        return TRUE;

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(h_main_window, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_GRAYTEXT + 1));
        EndPaint(h_main_window, &ps);
        return TRUE;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == start_button)
        {
            on_start_button_click();
        }
        return TRUE;
    }

    default:
        return DefWindowProc(h_main_window, uMsg, wParam, lParam);
    }
    return TRUE;
}

void main_window::on_start_button_click()
{
    HDC hdc = GetDC(NULL);

    int screenx = GetSystemMetrics(SM_CXSCREEN);
    int screeny = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, screenx, screeny);
    HDC hdcMem = CreateCompatibleDC(hdc);

    SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, screenx, screeny, hdc, 0, 0, SRCCOPY);
    save_file(L"./pic.bmp", hBitmap);

    ReleaseDC(h_main_window, hdcMem);
    ReleaseDC(h_main_window, hdc);

    MessageBox(h_main_window, L"start button clicked!", L"info", MB_OK);
}

BOOL main_window::save_file(LPCTSTR lpszFilePath, HBITMAP hBm)
{
    //  定义位图文件表头
    BITMAPFILEHEADER bmfh;
    //  定义位图信息表头
    BITMAPINFOHEADER bmih;

    //  调色板长度
    int nColorLen = 0;
    //  调色表大小
    DWORD dwRgbQuadSize = 0;
    //  位图大小
    DWORD dwBmSize = 0;
    //  分配内存的指针
    HGLOBAL hMem = NULL;

    LPBITMAPINFOHEADER lpbi;

    BITMAP bm;

    HDC hDC;

    HANDLE hFile = NULL;

    DWORD dwWritten;

    GetObject(hBm, sizeof(BITMAP), &bm);

    bmih.biSize = sizeof(BITMAPINFOHEADER); // 本结构所占的字节
    bmih.biWidth = bm.bmWidth;              // 位图宽
    bmih.biHeight = bm.bmHeight;            // 位图高
    bmih.biPlanes = 1;
    bmih.biBitCount = bm.bmBitsPixel; // 每一图素的位数
    bmih.biCompression = BI_RGB;      // 不压缩
    bmih.biSizeImage = 0;             //  位图大小
    bmih.biXPelsPerMeter = 0;
    bmih.biYPelsPerMeter = 0;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;

    //  计算位图图素数据区大小
    dwBmSize = 4 * ((bm.bmWidth * bmih.biBitCount + 31) / 32) * bm.bmHeight;

    //  如果图素位 <= 8bit，则有调色板
    if (bmih.biBitCount <= 8)
    {
        nColorLen = (1 << bm.bmBitsPixel);
    }

    //  计算调色板大小
    dwRgbQuadSize = nColorLen * sizeof(RGBQUAD);

    //  分配内存
    hMem = GlobalAlloc(GHND, dwBmSize + dwRgbQuadSize + sizeof(BITMAPINFOHEADER));

    if (NULL == hMem)
    {
        return FALSE;
    }

    //  锁定内存
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hMem);

    //  将bmih中的内容写入分配的内存中
    *lpbi = bmih;

    hDC = GetDC(NULL);

    //  将位图中的数据以bits的形式放入lpData中。
    GetDIBits(hDC, hBm, 0, (DWORD)bmih.biHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwRgbQuadSize, (BITMAPINFO *)lpbi, (DWORD)DIB_RGB_COLORS);

    bmfh.bfType = 0x4D42;                                                                         // 位图文件类型：BM
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwRgbQuadSize + dwBmSize; // 位图大小
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwRgbQuadSize; // 位图数据与文件头部的偏移量

    //  把上面的数据写入文件中

    hFile = CreateFile(lpszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        return FALSE;
    }

    //  写入位图文件头
    WriteFile(hFile, (LPSTR)&bmfh, sizeof(BITMAPFILEHEADER), (DWORD *)&dwWritten, NULL);
    //  写入位图数据
    WriteFile(hFile, (LPBITMAPINFOHEADER)lpbi, bmfh.bfSize - sizeof(BITMAPFILEHEADER), (DWORD *)&dwWritten, NULL);

    GlobalFree(hMem);
    CloseHandle(hFile);

    return TRUE;
}
