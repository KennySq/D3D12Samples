#pragma once

#include"inc\stdafx.h"
using namespace Microsoft::WRL;

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

	void SetAppName(std::string name);


private:
	ComPtr<ID3D12Device5> mDevice;
	ComPtr<IDXGISwapChain4> mSwapChain;

	uint mWidth;
	uint mHeight;

	float mAspectRatio;

	std::string mAppName;
};