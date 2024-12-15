#ifndef UNICODE
#define UNICODE
#endif
#define EXIT 128
#define ABUT 256
#define SAVE 512
#define FSIZ 1024
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "src.h"

// for win cbs:
// 128 << idx
// (assuming idx is 0-based)

int rX = 0;
int rY = 0;
int rXV = 5;
int rYV = 5;
HWND edit;

bool triggerSaveAs() {
	OPENFILENAME ofn;
	wchar_t szFile[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = L"All Files\0*.*\0";
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.nMaxFile = sizeof(szFile);
	ofn.nFilterIndex = 1;  // Default filter index
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL; // Initial directory
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn) != TRUE) {
		return false;
	}
	FILE* file = _wfopen(ofn.lpstrFile, L"w");
	if (file == NULL) {
		return true;
	}
	int len = SendMessageW(edit, WM_GETTEXTLENGTH, 0, 0);
	len++;
	wchar_t* buf = (wchar_t*)malloc(len * sizeof(wchar_t));
	SendMessageW(edit, WM_GETTEXT, len, (LPARAM)buf);
	fwprintf(file, L"%ls", buf);
	fclose(file);
	return true;
}

HMENU populateMenu() {
	HMENU res = CreateMenu();
	HMENU fileM = CreateMenu();
	HMENU editM = CreateMenu();
	// file
	AppendMenu(fileM, MF_STRING, EXIT, L"excite");
	AppendMenu(fileM, MF_STRING, ABUT, L"aboot");
	AppendMenu(fileM, MF_SEPARATOR, 0, NULL);
	AppendMenu(fileM, MF_STRING, SAVE, L"sane");
	// file
	// edit
	AppendMenu(editM, MF_STRING, FSIZ, L"fort sine....");
	// edit
	AppendMenu(res, MF_POPUP, (UINT_PTR)fileM, L"filet");
	AppendMenu(res, MF_POPUP, (UINT_PTR)editM, L"emit");
	return res;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	const wchar_t CLASS_NAME[] = L"TextEitorer";
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = CLASS_NAME;
	if (!RegisterClass(&wc)) {
		return 0;
	}
	HMENU menu = populateMenu();
	HWND hwnd = CreateWindowEx(
		0, // window styles
		CLASS_NAME, // class
		L"text eitorer", // title
		WS_TILED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // moar window styles ig
		CW_USEDEFAULT, CW_USEDEFAULT, // xy
		600, // width
		380, // height
		NULL, // parent
		menu,
		hInstance, // an instance of h (100% NOT CLICKBAIT!!!!)
		NULL // useless app data
	);
	if (hwnd == NULL) {
		return 0;
	}
	edit = CreateWindowEx(
		0,
		L"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
		0, 0, 600 - (GetSystemMetrics(SM_CXVSCROLL) / 2), 380 - (GetSystemMetrics(SM_CYVSCROLL) * 3),
		hwnd, NULL, hInstance, NULL
	);
	HFONT hFont = CreateFont(
		24, // is font tall?..
		0,
		0,
		0,
		FW_NORMAL, // ...is it fat?
		0, // italicisation
		0, // underline
		0, // strike
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DRAFT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"comic sans ms"
	);
	SendMessage(edit, WM_SETFONT, (WPARAM)hFont, 1);
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX);
	ShowWindow(hwnd, nCmdShow);
	SendMessage(hwnd, WM_SETICON, 0, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON)));
	// SetTimer(hwnd, 1, 1000 / 60, NULL);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

COLORREF hbrushToColorref(HBRUSH hBrush) {
	COLORREF col = CLR_INVALID;
	LOGBRUSH lb;
	int result = GetObject(hBrush, sizeof(LOGBRUSH), &lb);
	if ((result) && (lb.lbStyle == BS_SOLID)) col = lb.lbColor;
	return col;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case EXIT:
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case ABUT:
					MessageBox(hwnd, L"text eitorer\n\nmade by grad man 2 be silly", L"aboot", MB_OK | MB_ICONINFORMATION);
					break;
				case SAVE:
					triggerSaveAs();
					break;
				case FSIZ:
					CHOOSEFONT cf;
					LOGFONT lf;
					ZeroMemory(&cf, sizeof(cf));
					ZeroMemory(&lf, sizeof(lf));
					cf.lStructSize = sizeof(cf);
					cf.hwndOwner = hwnd;
					// cf.hInstance = 
					cf.lpLogFont = &lf;
					cf.Flags = CF_SCREENFONTS | CF_SHOWHELP | CF_SCALABLEONLY | CF_FORCEFONTEXIST;
					BOOL font = ChooseFont(&cf);
					break;
			}
			return 0;
		case WM_CLOSE:
			int result = MessageBoxW(hwnd, L"doh yo went two sane beform exciting text eitorer?", L"text eitorer", MB_YESNOCANCEL | MB_ICONWARNING);
			if ((result == IDCANCEL) || ((result == IDYES) && (!(triggerSaveAs())))) return 0;
			DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_CTLCOLOREDIT:
			HDC hdcEdit = (HDC)wParam;
			SetBkColor(hdcEdit, GetSysColor(COLOR_WINDOW));
			return (LRESULT)GetStockObject(BLACK_BRUSH);
		/*
			case WM_PAINT:
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
				// rectangle ig
				HBRUSH hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
				HPEN hPenInvisible = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOW));
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushRed);
				HPEN hOldPen = (HPEN)SelectObject(hdc, hPenInvisible);
				Rectangle(hdc, rX, rY, 100 + rX, 100 + rY);
				SelectObject(hdc, hOldBrush);
				SelectObject(hdc, hOldPen);
				DeleteObject(hBrushRed);
				DeleteObject(hPenInvisible);
				// rectangle ig
				EndPaint(hwnd, &ps);
				return 0;
			case WM_TIMER:
				rX += rXV;
				rY += rYV;
				InvalidateRect(hwnd, NULL, 1);
				return 0;
			case WM_KEYDOWN:
				if ((wParam == 'A') && (GetKeyState(VK_CONTROL) & 0x8000)) { // Check if Ctrl is pressed
					SendMessage(edit, EM_SETSEL, 0, -1); // Select all text in the edit control
					return 0;
				}
				break;
		*/
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}