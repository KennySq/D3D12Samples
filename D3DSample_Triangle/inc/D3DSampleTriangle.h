#pragma once
#include"inc/D3DSample.h"

using namespace DirectX;

class D3DSampleTriangle : public D3DSample
{
public:
	D3DSampleTriangle();

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update(float delta) override;
	virtual void Render(float delta) override;
	virtual void Release() override;
private:
	static const uint mFrameCount = 2;

	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT4 Color;
	};

	ComPtr<ID3D12Device6> mDevice;
	ComPtr<IDXGISwapChain3> mSwapChain;
	
	ComPtr<ID3D12Resource> mRenderTargets[mFrameCount];
	ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	uint mRtvDescSize;

	ComPtr<ID3D12CommandQueue> mCommandQueue;
	ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

	ComPtr<ID3D12Fence> mFence;
	uint mFrameIndex;
	HANDLE mFenceHandle;
	long long mFenceValue;

	void startPipeline();
	void loadAssets();
	void populateCommandList();
	void waitForPreviousFrame();
};