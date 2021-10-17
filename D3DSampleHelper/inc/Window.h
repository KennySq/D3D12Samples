#pragma once
#include"inc/stdafx.h"
#include<Windows.h>

class D3DSample;
class Window
{
public:
	static int Start(D3DSample* sample, HINSTANCE handleInst, int nCmdShow);

	static HWND GetHandle() { return mHandle; }
private:
	static LRESULT __stdcall WinProc(HWND hwnd, uint message, WPARAM wParam, LPARAM lParam);

	static HWND mHandle;

};