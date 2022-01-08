#pragma once
#include <Windows.h>
#include <tchar.h>
#include <gdiplus.h>
#include <map>
#include "Board.h"

#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3

class KuChess {
public:
	Board m_board;

	HRESULT Initialize(HINSTANCE hInstance);
private:
	const TCHAR* szWindowClass = _T("KuChess");
	const TCHAR* szTitle = _T("KuChess");
	HINSTANCE m_hInst;
	Gdiplus::Image* m_images[13];
	int m_mouse_x, m_mouse_y;
	int m_grabbed_square;
	int m_selected_square;
	size_t m_selected_n_moves;
	uint32_t m_selected_moves[50];

	static LRESULT CALLBACK s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd);
	LRESULT OnCreate(HWND hWnd);
	LRESULT OnMouseMove(HWND hWnd, LPARAM lParam);
	LRESULT OnLButtonDown(HWND hWnd, LPARAM lParam);
	LRESULT OnLButtonUp(HWND hWnd, LPARAM lParam);
};