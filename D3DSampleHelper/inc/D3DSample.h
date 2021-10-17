#pragma once

#include"inc\stdafx.h"
using namespace Microsoft::WRL;


inline std::string HrToString(HRESULT hr)
{
	char s_str[64] = {};
	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<uint>(hr));

	return std::string(s_str);
}

class HrException : public std::runtime_error
{
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), mHr(hr) {}
	HRESULT Error() const { return mHr; }

private:
	const HRESULT mHr;
};

inline void Throw(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw HrException(hr);
	}
}

class D3DSample
{
public:
	
	D3DSample(uint width, uint height, std::string appName);
	virtual ~D3DSample();

	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Update(float delta) = 0;
	virtual void Render(float delta) = 0;
	virtual void Release() = 0;

	virtual void GetKeyDown(uint key) {}
	virtual void GetKeyUp(uint key) {}

	uint GetWidth() const { return mWidth; }
	uint GetHeight() const { return mHeight; }

	const char* GetName() const { return mAppName.c_str(); }
protected:
	void AcquireHardware(IDXGIFactory1* factory, IDXGIAdapter1** outAdapter, bool requestHighPerformance = false);
	void SetAppName(std::string name);

	uint mWidth;
	uint mHeight;

private:
	ComPtr<ID3D12Device5> mDevice;
	ComPtr<IDXGISwapChain4> mSwapChain;


	float mAspectRatio;

	std::string mAppName;
};