#include"inc\stdafx.h"

#include "..\inc\D3DSample.h"
#include"inc\Window.h"

D3DSample::D3DSample(uint width, uint height, std::string appName)
	: mWidth(width), mHeight(height), mAppName(appName)
{
	char assetPath[512];
		
	mAspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

D3DSample::~D3DSample()
{
}

void D3DSample::AcquireHardware(IDXGIFactory1* factory, IDXGIAdapter1** outAdapter, bool requestHighPerformance)
{
	HRESULT result;

	outAdapter = nullptr;

	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<IDXGIFactory6> factory6;

	result = factory->QueryInterface(IID_PPV_ARGS(&factory6));

	if (result != S_OK)
	{
		throw std::runtime_error("failed to query factory6 interface.");
	}


	DXGI_GPU_PREFERENCE highPerformance = true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED;
	uint itr = 0;
	uint count = 0;
	do
	{
		count = factory6->EnumAdapterByGpuPreference(itr, highPerformance, IID_PPV_ARGS(&adapter));
		
		DXGI_ADAPTER_DESC1 desc;

		adapter->GetDesc1(&desc);
		
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		
		result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), nullptr);
		if (result == S_OK)
		{
			break;
		}

		itr++;
	} while (itr < count);
	

	if (adapter.Get() == nullptr)
	{
		result = factory->EnumAdapters1(0, &adapter);

		for (uint adapterIndex = 0; result == S_OK; adapterIndex++)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}

			result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), nullptr);
			if (result == S_OK)
			{
				break;
			}
		
		}
	}

	*outAdapter = adapter.Detach();

	return;
}

void D3DSample::SetAppName(std::string name)
{
	SetWindowText(Window::GetHandle(), name.c_str());
}

