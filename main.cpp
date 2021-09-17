#include <QtWidgets/QApplication>
#include "widget.h"

#include <Windows.h>

HHOOK keyHook;
Widget *main_widget = nullptr;

LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lParam;

    if (pkbhs->vkCode == VK_ESCAPE)
    {
        main_widget->pause_key_down();
    }
    return CallNextHookEx(keyHook, nCode, wParam, lParam);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    main_widget = &w;
    keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyProc, nullptr, 0);
    return a.exec();
}