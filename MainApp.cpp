#include <sdkddkver.h>
#include <afxwin.h>

#include "MainFrame.h"

class CMyWinApp : public CWinApp
{
    BOOL InitInstance() override
    {
        auto main_frame_wnd = new CMainFrameWnd;
        // init ocr here
        if (!main_frame_wnd->InitOCR())
        {
            return FALSE;
        }
        RECT wnd_rect = CMainFrameWnd::client;
        DWORD wnd_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        AdjustWindowRect(&wnd_rect, wnd_style, false);

        main_frame_wnd->Create(nullptr, TEXT("腐烂国度2摇人小工具"), wnd_style, wnd_rect);
        m_pMainWnd = main_frame_wnd;
        m_pMainWnd->ShowWindow(m_nCmdShow);
        m_pMainWnd->UpdateWindow();
        m_pMainWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        return TRUE;
    }
};

CMyWinApp app;
