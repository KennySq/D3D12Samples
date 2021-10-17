#include"inc/Window.h"
#include"inc/D3DSample.h"

#include<atlbase.h>

int Window::Start(D3DSample* sample, HINSTANCE handleInst, int nCmdShow)
{
	WNDCLASSEX winClass = {};

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WinProc;
	winClass.hInstance = handleInst;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.lpszClassName = L"D3D12Sample";
	RegisterClassEx(&winClass);

	RECT winRect = { 0,0, static_cast<long>(sample->GetWidth()), static_cast<long>(sample->GetHeight()) };

	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, false);

	USES_CONVERSION;

	mHandle = CreateWindow(winClass.lpszClassName, A2W(sample->GetName()), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, winRect.right - winRect.left, winRect.bottom - winRect.top, nullptr, nullptr, handleInst, sample);

	if (mHandle == nullptr)
	{
		throw std::runtime_error("failed to init window handle.");
	}

	sample->Start();

	MSG msg{};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	sample->Release();

	return static_cast<char>(msg.wParam);
}

LRESULT __stdcall Window::WinProc(HWND hwnd, uint message, WPARAM wParam, LPARAM lParam)
{
	D3DSample* sample = reinterpret_cast<D3DSample*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	// GWLP_USERDATA?? WTF.



}
