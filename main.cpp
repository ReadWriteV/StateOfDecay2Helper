#include <Windows.h>

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, INT nCmdShow)
{
    MessageBox(NULL, L"hello world", L"Test", MB_OK);
    return 0;
}