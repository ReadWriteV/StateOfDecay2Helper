#pragma once

#include <Windows.h>

template <typename Derived_Type>
class base_window
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        Derived_Type *dervied_this = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
            dervied_this = reinterpret_cast<Derived_Type *>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(dervied_this));

            dervied_this->hwnd = hwnd;
        }
        else
        {
            dervied_this = reinterpret_cast<Derived_Type *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }
        if (dervied_this)
        {
            return dervied_this->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    base_window() : hwnd(NULL) {}

    BOOL Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle = 0,
                int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT,
                HWND hWndParent = 0, HMENU hMenu = 0)
    {
        WNDCLASS wc = {0};

        wc.lpfnWndProc = Derived_Type::WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();

        RegisterClass(&wc);

        hwnd = CreateWindowEx(
            dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this);

        return (hwnd ? TRUE : FALSE);
    }

    HWND Window() const { return hwnd; }

protected:
    virtual PCWSTR ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

private:
    HWND hwnd;
};