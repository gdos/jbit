/*
 * Copyright (C) 2014  Emanuele Fornara
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <windows.h>

#include "../embd/embd.h"

namespace {

uint8_t video[LCD_WIDTH * LCD_HEIGHT]; // TODO: padding

const char *szClassName = "jbit";
LPBITMAPINFO pDIB;

void create() {
	memset(video, 0, sizeof(video));
	for (int i = 0; i < 200; i++)
		video[i] = i;
}

void paint(HDC dc) {
	StretchDIBits(dc,
	  10, 10, LCD_WIDTH * 2, LCD_HEIGHT * 2,
	  0, 0, LCD_WIDTH, LCD_HEIGHT,
	  video, pDIB,
	  DIB_RGB_COLORS, SRCCOPY);
}

void dib_create() {
	DWORD size = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;
	pDIB = (LPBITMAPINFO)new BYTE[size];
	pDIB->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pDIB->bmiHeader.biWidth = LCD_WIDTH;
	pDIB->bmiHeader.biHeight = -LCD_HEIGHT;
	pDIB->bmiHeader.biBitCount = 1;
	pDIB->bmiHeader.biPlanes = 1;
	pDIB->bmiHeader.biCompression = BI_RGB;
	pDIB->bmiHeader.biXPelsPerMeter = 1000;
	pDIB->bmiHeader.biYPelsPerMeter = 1000;
	pDIB->bmiHeader.biClrUsed = 0;
	pDIB->bmiHeader.biClrImportant = 0;
	RGBQUAD *pColors = (RGBQUAD *)&((BYTE *)pDIB)[sizeof(BITMAPINFOHEADER)];
	pColors[0].rgbRed = 0x96;
	pColors[0].rgbGreen = 0xbb;
	pColors[0].rgbBlue = 0xa4;
	pColors[0].rgbReserved = 0;
	pColors[1].rgbRed = 0x00;
	pColors[1].rgbGreen = 0x00;
	pColors[1].rgbBlue = 0x00;
	pColors[1].rgbReserved = 0;
}

void dib_destroy() {
	delete[] (BYTE *)pDIB;
}

} // namespace;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
		dib_create();
		create();
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		paint(hdc);
		EndPaint(hWnd, &ps);
		} break;
	case WM_DESTROY:
		dib_destroy();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow) {

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, (char *)IDI_APPLICATION);
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szClassName;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, (char *)IDI_APPLICATION);
	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(
		szClassName,
		"JBit " JBIT_VERSION,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		200, 300,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
