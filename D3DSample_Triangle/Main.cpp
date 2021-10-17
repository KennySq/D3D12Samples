#include"inc/stdafx.h"
#include"inc/D3DSampleTriangle.h"

int __stdcall WinMain(HINSTANCE handleInst, HINSTANCE prevInstance, LPSTR message, int nCmdShow)
{
	D3DSampleTriangle sample;
	return Window::Start(&sample, handleInst, nCmdShow);
}