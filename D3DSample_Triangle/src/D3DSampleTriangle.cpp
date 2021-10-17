#include"inc/stdafx.h"
#include"inc/D3DSampleTriangle.h"

D3DSampleTriangle::D3DSampleTriangle()
	: D3DSample(1280, 720, "D3DSample_Triangle")
{

}

void D3DSampleTriangle::Awake()
{


}

void D3DSampleTriangle::Start()
{
}

void D3DSampleTriangle::Update(float delta)
{
}

void D3DSampleTriangle::Render(float delta)
{
}

void D3DSampleTriangle::Release()
{
}

void D3DSampleTriangle::startPipeline()
{
	HRESULT result;
	uint dxgiFactoryFlags = 0;

#ifdef _DEBUG
	ComPtr<ID3D12Debug> debugController;
	
	Throw(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));

	if (result == S_OK)
	{
		debugController->EnableDebugLayer();

		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ComPtr<IDXGIFactory4> factory;
	result = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));

	ComPtr<IDXGIAdapter1> adapter;
	AcquireHardware(factory.Get(), &adapter);

	Throw(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice)));

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};

	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	Throw(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	swapChainDesc.BufferCount = mFrameCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Width = mWidth;
	swapChainDesc.Height = mHeight;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SampleDesc.Count = 1;

	HWND hWnd = Window::GetHandle();

	ComPtr<IDXGISwapChain1> swapchain;

	Throw(factory->CreateSwapChainForHwnd(mDevice.Get(), hWnd, &swapChainDesc, nullptr, nullptr, swapchain.GetAddressOf()));
	Throw(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

	Throw(swapchain.As(&mSwapChain));

	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NumDescriptors = mFrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		Throw(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap)));

		mRtvDescSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};

		rtvHandle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();

		for (uint i = 0; i < mFrameCount; i++)
		{
			Throw(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mRenderTargets[i])));

			mDevice->CreateRenderTargetView(mRenderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.ptr = rtvHandle.ptr + mRtvDescSize;
		}
	}

	Throw(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));
}

void D3DSampleTriangle::loadAssets()
{

	{
		D3D12_ROOT_SIGNATURE_DESC rootSignDesc{};

		rootSignDesc.NumParameters = 0;
		rootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignDesc.NumStaticSamplers = 0;
		rootSignDesc.pParameters = nullptr;
		rootSignDesc.pStaticSamplers = nullptr;

		ComPtr<ID3DBlob> sign;
		ComPtr<ID3DBlob> error;

		Throw(D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, sign.GetAddressOf(), error.GetAddressOf()));
		Throw(mDevice->CreateRootSignature(0, sign->GetBufferPointer(), sign->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
	}

	{
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

#ifdef _DEBUG
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		Throw(D3DCompileFromFile(L"SampleTriangle.hlsl", nullptr, nullptr, "vert", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
		Throw(D3DCompileFromFile(L"SampleTriangle.hlsl", nullptr, nullptr, "frag", "ps_5_0", compileFlags, 0, &vertexShader, nullptr));

		D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = 
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		D3D12_SHADER_BYTECODE vertexBytecode = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
		D3D12_SHADER_BYTECODE pixelBytecode = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };

		D3D12_RASTERIZER_DESC rasterDesc{};

		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterDesc.CullMode = D3D12_CULL_MODE_BACK;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0;
		rasterDesc.SlopeScaledDepthBias = 0;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.ForcedSampleCount = 0;
		rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		D3D12_BLEND_DESC blendDesc{};

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		{
			D3D12_RENDER_TARGET_BLEND_DESC rtvBlendDesc{};

			rtvBlendDesc.BlendEnable = false;
			rtvBlendDesc.LogicOpEnable = false;
			rtvBlendDesc.SrcBlend = D3D12_BLEND_ONE;
			rtvBlendDesc.DestBlend = D3D12_BLEND_ZERO;
			rtvBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
			rtvBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
			rtvBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
			rtvBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			rtvBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
			rtvBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; 

			for (uint i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
			{
				blendDesc.RenderTarget[i] = rtvBlendDesc;
			}
		}


		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};

		psoDesc.InputLayout = { inputElementDesc, ARRAYSIZE(inputElementDesc) };
		psoDesc.pRootSignature = mRootSignature.Get();
		psoDesc.VS = vertexBytecode;
		psoDesc.PS = pixelBytecode;
		psoDesc.RasterizerState = rasterDesc;
		psoDesc.BlendState = blendDesc;
		psoDesc.DepthStencilState.DepthEnable = false;
		psoDesc.DepthStencilState.StencilEnable = false;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;

		Throw(mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));

		Throw(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));

		{

		}


	}

}

void D3DSampleTriangle::populateCommandList()
{
}

void D3DSampleTriangle::waitForPreviousFrame()
{
}
