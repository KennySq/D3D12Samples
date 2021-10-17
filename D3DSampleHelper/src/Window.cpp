#include"inc/Window.h"
#include"inc/D3DSample.h"

HWND Window::mHandle = nullptr;

int Window::Start(D3DSample* sample, HINSTANCE handleInst, int nCmdShow)
{
	WNDCLASSEX winClass = {};

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WinProc;
	winClass.hInstance = handleInst;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.lpszClassName = "D3D12Sample";
	RegisterClassEx(&winClass);

	RECT winRect = { 0,0, static_cast<long>(sample->GetWidth()), static_cast<long>(sample->GetHeight()) };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, false);

	mHandle = CreateWindow(winClass.lpszClassName, sample->GetName(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, winRect.right - winRect.left, winRect.bottom - winRect.top, nullptr, nullptr, handleInst, sample);

	if (mHandle == nullptr)
	{
		throw std::runtime_error("failed to init window handle.");
	}

	sample->Awake();
	sample->Start();

	ShowWindow(mHandle, nCmdShow);





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

LRESULT __stdcall Window::WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	D3DSample* sample = reinterpret_cast<D3DSample*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	// GWLP_USERDATA?? WTF.

	switch (message)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<long long>(createStruct->lpCreateParams));
	}
	return 0;

	case WM_KEYDOWN:
	{
		if (sample)
		{
			sample->GetKeyDown(static_cast<byte>(wParam));
		}
	}
	return 0;

	case WM_KEYUP:
	{
		if (sample)
		{
			sample->GetKeyUp(static_cast<byte>(lParam));
		}
	}
	return 0;

	case WM_PAINT:
	{
		if (sample)
		{
			sample->Update(0);
			sample->Render(0);
		}

	}
	return 0;

	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);

}
