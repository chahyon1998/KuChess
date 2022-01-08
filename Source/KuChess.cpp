#include "KuChess.h"
#include <iostream>
#include <cassert>
#include <windowsx.h>

HRESULT KuChess::Initialize(HINSTANCE hInstance)
{
    m_images[NN] = new Gdiplus::Image(_T("Image/board.png"));
    m_images[BP] = new Gdiplus::Image(_T("Image/bp.png"));
    m_images[BB] = new Gdiplus::Image(_T("Image/bb.png"));
    m_images[BN] = new Gdiplus::Image(_T("Image/bn.png"));
    m_images[BR] = new Gdiplus::Image(_T("Image/br.png"));
    m_images[BQ] = new Gdiplus::Image(_T("Image/bq.png"));
    m_images[BK] = new Gdiplus::Image(_T("Image/bk.png"));
    m_images[WP] = new Gdiplus::Image(_T("Image/wp.png"));
    m_images[WB] = new Gdiplus::Image(_T("Image/wb.png"));
    m_images[WN] = new Gdiplus::Image(_T("Image/wn.png"));
    m_images[WR] = new Gdiplus::Image(_T("Image/wr.png"));
    m_images[WQ] = new Gdiplus::Image(_T("Image/wq.png"));
    m_images[WK] = new Gdiplus::Image(_T("Image/wk.png"));
    m_grabbed_square = -1;
    m_selected_square = -1;

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = KuChess::s_WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(2);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("KuChess"),
            NULL);

        return E_FAIL;
    }

    m_hInst = hInstance;

    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1200, 900,
        NULL,
        NULL,
        hInstance,
        this
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("KuChess"),
            NULL);

        return E_FAIL;
    }

    SetTimer(hWnd, 1, 10, NULL);

    ShowWindow(hWnd, 1);
    UpdateWindow(hWnd);

    return S_OK;
}

LRESULT KuChess::s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE)
    {
        LPCREATESTRUCT pcs = reinterpret_cast<LPCREATESTRUCT> (lParam);
        KuChess* pThis = reinterpret_cast<KuChess*> (pcs->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    else
    {
        KuChess* pThis = reinterpret_cast<KuChess*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (pThis)
            return pThis->WndProc(hWnd, message, wParam, lParam);
        else
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

LRESULT KuChess::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        return OnPaint(hWnd);
    case WM_CREATE:
        return OnCreate(hWnd);
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    case WM_COMMAND:
        break;
    case WM_MOUSEMOVE:
        return OnMouseMove(hWnd, lParam);
    case WM_LBUTTONDOWN:
        return OnLButtonDown(hWnd, lParam);
    case WM_LBUTTONUP:
        return OnLButtonUp(hWnd, lParam);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT KuChess::OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    RECT client_rect;
    GetClientRect(hWnd, &client_rect);
    int client_width = client_rect.right - client_rect.left;
    int client_height = client_rect.bottom - client_rect.top;
    HDC hdc_mem = CreateCompatibleDC(hdc);
    HBITMAP hbm_mem = CreateCompatibleBitmap(hdc, client_width, client_height);

    HANDLE hbm_old = SelectObject(hdc_mem, hbm_mem);
    Gdiplus::Graphics graphics(hdc_mem);

    Gdiplus::SolidBrush white_brush(RGB(0, 0, 0));
    graphics.FillRectangle(&white_brush, 0, 0, client_width, client_height);

    int board_width = 600;
    int board_height = 600;
    graphics.DrawImage(m_images[NN], 0, 0, board_width, board_height);
    for (int i = 0; i < 64; ++i) {
        int x = i % 8;
        int y = i / 8;
        int piece = m_board.m_squares_piece[i];
        if (i != m_grabbed_square && piece != 0) {
            graphics.DrawImage(m_images[piece], x * 75, y * 75, 75, 75);
        }
    }
    if (m_grabbed_square >= 0) {
        int piece = m_board.m_squares_piece[m_grabbed_square];
        assert(piece > 0);
        graphics.DrawImage(m_images[piece], m_mouse_x - 75 / 2, m_mouse_y - 75 / 2, 75, 75);
    }
    if (m_selected_square >= 0) {
        Gdiplus::Pen select_pen(Gdiplus::Color(255, 255, 255), 5);
        int x = m_selected_square % 8;
        int y = m_selected_square / 8;
        graphics.DrawRectangle(&select_pen, 75 * x, 75 * y, 75, 75);
        for (int i = 0; i < m_selected_n_moves; ++i) {
            int move = m_selected_moves[i];
            int dst = DST(move);
            int dst_x = dst % 8;
            int dst_y = dst / 8;
            graphics.DrawEllipse(&select_pen, dst_x * 75 + 20, dst_y * 75 + 20, 35, 35);
        }
    }

    BitBlt(hdc, 0, 0, client_width, client_height, hdc_mem, 0, 0, SRCCOPY);
    SelectObject(hdc_mem, hbm_old);
    DeleteObject(hbm_mem);
    DeleteDC(hdc_mem);

    EndPaint(hWnd, &ps);
    return 0;
}

LRESULT KuChess::OnCreate(HWND hWnd) {
    HMENU hMenubar;
    HMENU hMenu;

    hMenubar = CreateMenu();
    hMenu = CreateMenu();

    AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
    AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");

    AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File");
    SetMenu(hWnd, hMenubar);
    return 0;
}

LRESULT KuChess::OnMouseMove(HWND hWnd, LPARAM lParam) {
    m_mouse_x = GET_X_LPARAM(lParam);
    m_mouse_y = GET_Y_LPARAM(lParam);

    if (m_grabbed_square >= 0) {
        assert(m_board.m_squares_piece[m_index_grabbed] != 0);
        HCURSOR grab_cursor = LoadCursor(NULL, IDC_HAND);
        SetCursor(grab_cursor);
        const RECT bound = RECT{ m_mouse_x - 75 / 2, m_mouse_y - 75 / 2, m_mouse_x + 75 / 2, m_mouse_y + 75 / 2 };
    }
    else {
        int board_size = 600;
        if (0 <= m_mouse_x && m_mouse_x < board_size && 0 <= m_mouse_y && m_mouse_y < board_size) {
            int x = m_mouse_x * 8 / board_size;
            int y = m_mouse_y * 8 / board_size;
            assert(0 <= x && x < 8 && 0 <= y && y < 8);
            if (m_board.m_squares_piece[x + 8 * y] != 0) {
                HCURSOR grab_cursor = LoadCursor(NULL, IDC_HAND);
                SetCursor(grab_cursor);
                return 0;
            }
        }
        HCURSOR default_cursor = LoadCursor(NULL, IDC_ARROW);
        SetCursor(default_cursor);
        return 0;
    }
}

LRESULT KuChess::OnLButtonDown(HWND hWnd, LPARAM lParam)
{
    assert(m_index_grabbed == -1);
    int m_mouse_x = GET_X_LPARAM(lParam);
    int m_mouse_y = GET_Y_LPARAM(lParam);
    SetCapture(hWnd);
    int board_size = 600;
    if (0 <= m_mouse_x && m_mouse_x < board_size && 0 <= m_mouse_y && m_mouse_y < board_size) {
        int x = m_mouse_x * 8 / board_size;
        int y = m_mouse_y * 8 / board_size;
        assert(0 <= x && x < 8 && 0 <= y && y < 8);

        m_selected_square = x + y * 8;
        m_board.GetLegalMoves(m_selected_square, m_selected_moves, &m_selected_n_moves, 100);

        if (m_board.m_squares_piece[x + 8 * y] != 0) {
            HCURSOR grab_cursor = LoadCursor(NULL, IDC_HAND);
            SetCursor(grab_cursor);
            m_grabbed_square = x + 8 * y;
            return 0;
        }
    }
    return 0;
}

LRESULT KuChess::OnLButtonUp(HWND hWnd, LPARAM lParam)
{
    ReleaseCapture();
    int m_mouse_x = GET_X_LPARAM(lParam);
    int m_mouse_y = GET_Y_LPARAM(lParam);

    m_grabbed_square = -1;

    if (m_selected_square >= 0) {
        int board_size = 600;
        if (0 <= m_mouse_x && m_mouse_x < board_size && 0 <= m_mouse_y && m_mouse_y < board_size) {
            int x = m_mouse_x * 8 / board_size;
            int y = m_mouse_y * 8 / board_size;
            assert(0 <= x && x < 8 && 0 <= y && y < 8);

            int dst = x + y * 8;
            for (int i = 0; i < m_selected_n_moves; ++i) {
                if (dst == DST(m_selected_moves[i])) {
                    m_board.Move(MOVE(m_board.m_squares_piece[m_selected_square], m_selected_square, dst));
                    m_selected_square = -1;
                }
            }
            m_board.GetLegalMoves(m_selected_square, m_selected_moves, &m_selected_n_moves, 100);
        }
    }

    return 0;
}