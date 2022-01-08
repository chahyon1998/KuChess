#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include "KuChess.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE,
    _In_ LPSTR,
    _In_ int
) {
    // Initialize GDI+.
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    // Checking return status from GdiplusStartup 
    Gdiplus::Status status = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    if (status == Gdiplus::Ok) {
        KuChess* kuChess = new KuChess();
        kuChess->m_board.Reset();
        kuChess->m_board.Print();
        HRESULT hr = kuChess->Initialize(hInstance);

        if (SUCCEEDED(hr)) {
            BOOL bRet;
            MSG msg;

            while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
                if (bRet == -1) {
                    break;
                } else {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }

        Gdiplus::GdiplusShutdown(gdiplusToken);
    }
}