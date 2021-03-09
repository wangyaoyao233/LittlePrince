/*system header*/
#include <io.h>
#include "Main.h"
/*tools header*/
#include "DXTrace.h"
#include "d3dUtil.h"
/*scenes header*/
/*gameobjects header*/
/*self header*/
#include "Renderer.h"

ComPtr<ID3D11Device> Renderer::m_D3DDevice;
ComPtr<ID3D11Device> Renderer::m_D3DDevice1;
ComPtr<ID3D11DeviceContext> Renderer::m_ImmediateContext;
ComPtr<ID3D11DeviceContext> Renderer::m_ImmediateContext1;
ComPtr<IDXGISwapChain> Renderer::m_pSwapChain;
ComPtr<IDXGISwapChain1> Renderer::m_pSwapChain1;
ComPtr<ID3D11RenderTargetView> Renderer::m_RenderTargetView;
ComPtr<ID3D11DepthStencilView> Renderer::m_DepthStencilView;
ComPtr<ID3D11Texture2D> Renderer::m_DepthStencilBuffer;

ComPtr<ID3D11Buffer> Renderer::m_WorldBuffer;
ComPtr<ID3D11Buffer> Renderer::m_ViewBuffer;
ComPtr<ID3D11Buffer> Renderer::m_ProjectionBuffer;
ComPtr<ID3D11Buffer> Renderer::m_MaterialBuffer;
ComPtr<ID3D11Buffer> Renderer::m_CameraBuffer;
ComPtr<ID3D11Buffer> Renderer::m_ParameterBuffer;

ComPtr<ID3D11SamplerState> Renderer::m_pSamplerState;// 采样器


UINT Renderer::m_4xMsaaQuality;

bool Renderer::Init()
{
	CoInitialize(nullptr); // WICTextureLoader

	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// driver type
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);
	D3D_DRIVER_TYPE d3dDriverType;

	// feature level
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	D3D_FEATURE_LEVEL featureLevel;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, m_D3DDevice.GetAddressOf(), &featureLevel, m_ImmediateContext.GetAddressOf());

		if (hr == E_INVALIDARG)
		{
			// Direct3D 11.0 的API不承认D3D_FEATURE_LEVEL_11_1，所以我们需要尝试特性等级11.0以及以下的版本
			hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1, D3D11_SDK_VERSION, m_D3DDevice.GetAddressOf(), &featureLevel, m_ImmediateContext.GetAddressOf());
		}

		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	// 检测是否支持特性等级11.0或11.1
	if (featureLevel != D3D_FEATURE_LEVEL_11_0 && featureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// 检测 MSAA支持的质量等级
	m_D3DDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality);
	assert(m_4xMsaaQuality > 0);


	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
	ComPtr<IDXGIFactory1> dxgiFactory1 = nullptr;// D3D11.0(包含DXGI1.1)的接口类
	ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;// D3D11.1(包含DXGI1.2)特有的接口类

	// 为了正确创建 DXGI交换链，首先我们需要获取创建 D3D设备 的 DXGI工厂，否则会引发报错：
	// "IDXGIFactory::CreateSwapChain: This function is being called with a device from a different IDXGIFactory."
	HR(m_D3DDevice.As(&dxgiDevice));
	HR(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory1.GetAddressOf())));

	// 查看该对象是否包含IDXGIFactory2接口
	hr = dxgiFactory1.As(&dxgiFactory2);
	// 如果包含，则说明支持D3D11.1
	if (dxgiFactory2 != nullptr)
	{
		HR(m_D3DDevice.As(&m_D3DDevice1));
		HR(m_ImmediateContext.As(&m_ImmediateContext1));

		// 填充各种结构体用以描述交换链
		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Width = SCREEN_WIDTH;
		sd.Height = SCREEN_HEIGHT;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd;
		fd.RefreshRate.Numerator = 60;
		fd.RefreshRate.Denominator = 1;
		fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fd.Windowed = TRUE;

		// 为当前窗口创建交换链
		HR(dxgiFactory2->CreateSwapChainForHwnd(m_D3DDevice.Get(), GetWindow(), &sd, &fd, nullptr, m_pSwapChain1.GetAddressOf()));
		HR(m_pSwapChain1.As(&m_pSwapChain));
	}
	else 
	{
		// 填充DXGI_SWAP_CHAIN_DESC用以描述交换链
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferDesc.Width = SCREEN_WIDTH;
		sd.BufferDesc.Height = SCREEN_HEIGHT;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = GetWindow();
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;
		HR(dxgiFactory1->CreateSwapChain(m_D3DDevice.Get(), &sd, m_pSwapChain.GetAddressOf()));
	}

	// 可以禁止alt+enter全屏
	dxgiFactory1->MakeWindowAssociation(GetWindow(), DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

	// 设置调试对象名
	D3D11SetDebugObjectName(m_ImmediateContext.Get(), "ImmediateContext");
	DXGISetDebugObjectName(m_pSwapChain.Get(), "SwapChain");

	// 每当窗口被重新调整大小的时候，都需要调用这个OnResize函数。现在调用
	// 以避免代码重复
	OnResize();

	// todo
	// create constant buffer
	CreateConstantBuffer();

	// 光栅化状态设定 RasterizerState
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = FALSE;

	ComPtr<ID3D11RasterizerState> rs;
	m_D3DDevice->CreateRasterizerState(&rd, rs.GetAddressOf());
	m_ImmediateContext->RSSetState(rs.Get());


	// 设置采样器
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(m_D3DDevice->CreateSamplerState(&sampDesc, m_pSamplerState.GetAddressOf()));
	m_ImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

	return true;
}

void Renderer::Uninit()
{
	if (m_ImmediateContext)
	{
		m_ImmediateContext->ClearState();
	}
}

void Renderer::Begin()
{
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	m_ImmediateContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::End()
{
	HR(m_pSwapChain->Present(0, 0));
}

void Renderer::OnResize()
{
	assert(m_ImmediateContext);
	assert(m_D3DDevice);
	assert(m_pSwapChain);

	if (m_D3DDevice1 != nullptr)
	{
		assert(m_ImmediateContext1);
		assert(m_D3DDevice1);
		assert(m_pSwapChain1);
	}

	// 释放渲染管线输出用到的相关资源
	m_RenderTargetView.Reset();
	m_DepthStencilView.Reset();
	m_DepthStencilBuffer.Reset();

	// 重设交换链并且重新创建渲染目标视图
	ComPtr<ID3D11Texture2D> backBuffer;
	HR(m_pSwapChain->ResizeBuffers(1, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	HR(m_D3DDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RenderTargetView.GetAddressOf()));

	D3D11SetDebugObjectName(backBuffer.Get(), "BackBuffer[0]");


	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = SCREEN_WIDTH;
	depthStencilDesc.Height = SCREEN_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 4;
	depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// 创建深度缓冲区以及深度模板视图
	HR(m_D3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, m_DepthStencilBuffer.GetAddressOf()));
	HR(m_D3DDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, m_DepthStencilView.GetAddressOf()));

	// 将渲染目标视图和深度/模板缓冲区结合到管线
	m_ImmediateContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());

	D3D11_VIEWPORT viewPort;
	// 设置视口变换
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = static_cast<float>(SCREEN_WIDTH);
	viewPort.Height = static_cast<float>(SCREEN_HEIGHT);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	m_ImmediateContext->RSSetViewports(1, &viewPort);

	// 设置调试对象名
	D3D11SetDebugObjectName(m_DepthStencilBuffer.Get(), "DepthStencilBuffer");
	D3D11SetDebugObjectName(m_DepthStencilView.Get(), "DepthStencilView");
	D3D11SetDebugObjectName(m_RenderTargetView.Get(), "BackBufferRTV[0]");
}

void Renderer::SetWorldViewProjection2D()
{
	XMMATRIX world = XMMatrixIdentity();
	world = XMMatrixTranspose(world);

	m_ImmediateContext->UpdateSubresource(m_WorldBuffer.Get(), 0, nullptr, &world, 0, 0);

	XMMATRIX view = XMMatrixIdentity();
	view = XMMatrixTranspose(view);
	m_ImmediateContext->UpdateSubresource(m_ViewBuffer.Get(), 0, nullptr, &view, 0, 0);

	XMMATRIX projection = XMMatrixOrthographicOffCenterLH(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	projection = XMMatrixTranspose(projection);
	m_ImmediateContext->UpdateSubresource(m_ProjectionBuffer.Get(), 0, nullptr, &projection, 0, 0);
}

void Renderer::SetWorldMatrix(XMMATRIX WorldMatrix)
{
	WorldMatrix = XMMatrixTranspose(WorldMatrix);
	m_ImmediateContext->UpdateSubresource(m_WorldBuffer.Get(), 0, nullptr, &WorldMatrix, 0, 0);
}

void Renderer::SetViewMatrix(XMMATRIX ViewMatrix)
{
	ViewMatrix = XMMatrixTranspose(ViewMatrix);
	m_ImmediateContext->UpdateSubresource(m_ViewBuffer.Get(), 0, nullptr, &ViewMatrix, 0, 0);
}

void Renderer::SetProjectionMatrix(XMMATRIX ProjectionMatrix)
{
	ProjectionMatrix = XMMatrixTranspose(ProjectionMatrix);
	m_ImmediateContext->UpdateSubresource(m_ProjectionBuffer.Get(), 0, nullptr, &ProjectionMatrix, 0, 0);
}

void Renderer::SetCameraPosition(XMFLOAT3 CameraPos)
{
	XMFLOAT4 pos(CameraPos.x, CameraPos.y, CameraPos.z, 1.0f);
	m_ImmediateContext->UpdateSubresource(m_CameraBuffer.Get(), 0, nullptr, &pos, 0, 0);
}

void Renderer::SetParameter(XMFLOAT4 Parameter)
{
	m_ImmediateContext->UpdateSubresource(m_ParameterBuffer.Get(), 0, nullptr, &Parameter, 0, 0);
}

void Renderer::SetMaterial(MATERIAL& Material)
{
	m_ImmediateContext->UpdateSubresource(m_MaterialBuffer.Get(), 0, nullptr, &Material, 0, 0);
}

void Renderer::CreateVertexShader(ID3D11VertexShader** vertexShader, ID3D11InputLayout** vertexLayout, std::wstring fileName)
{
	FILE* file = nullptr;
	long int fsize;

	file = _wfopen(fileName.c_str(), L"rb");
	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_D3DDevice->CreateVertexShader(buffer, fsize, nullptr, vertexShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);
	m_D3DDevice->CreateInputLayout(layout, numElements, buffer, fsize, vertexLayout);

	delete[] buffer;
}

void Renderer::CreatePixelShader(ID3D11PixelShader** pixelShader, std::wstring fileName)
{
	FILE* file = nullptr;
	long int fsize;

	file = _wfopen(fileName.c_str(), L"rb");
	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_D3DDevice->CreatePixelShader(buffer, fsize, nullptr, pixelShader);

	delete[] buffer;
}

void Renderer::CreateConstantBuffer()
{
	// 设置常量缓冲区描述
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	/* Dynamic */
	//cbd.Usage = D3D11_USAGE_DYNAMIC;
	//cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	/* Default */
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0;

	cbd.ByteWidth = sizeof(XMMATRIX);
	// 新建常量缓冲区
	// WorldBuffer--> b0
	m_D3DDevice->CreateBuffer(&cbd, nullptr, m_WorldBuffer.GetAddressOf());
	m_ImmediateContext->VSSetConstantBuffers(0, 1, m_WorldBuffer.GetAddressOf());
	// ViewBuffer--> b1
	m_D3DDevice->CreateBuffer(&cbd, nullptr, m_ViewBuffer.GetAddressOf());
	m_ImmediateContext->VSSetConstantBuffers(1, 1, m_ViewBuffer.GetAddressOf());
	// ProjectionBuffer--> b2
	m_D3DDevice->CreateBuffer(&cbd, nullptr, m_ProjectionBuffer.GetAddressOf());
	m_ImmediateContext->VSSetConstantBuffers(2, 1, m_ProjectionBuffer.GetAddressOf());

	// MaterialBuffer--> b3
	cbd.ByteWidth = sizeof(MATERIAL);
	m_D3DDevice->CreateBuffer(&cbd, nullptr, m_MaterialBuffer.GetAddressOf());
	m_ImmediateContext->VSSetConstantBuffers(3, 1, m_MaterialBuffer.GetAddressOf());
	m_ImmediateContext->PSSetConstantBuffers(3, 1, m_MaterialBuffer.GetAddressOf());

	// CameraBuffer--> b4
	cbd.ByteWidth = sizeof(XMFLOAT4);
	m_D3DDevice->CreateBuffer(&cbd, nullptr, m_CameraBuffer.GetAddressOf());
	m_ImmediateContext->VSSetConstantBuffers(4, 1, m_CameraBuffer.GetAddressOf());
	m_ImmediateContext->PSSetConstantBuffers(4, 1, m_CameraBuffer.GetAddressOf());

	// Parameter--> b5
	cbd.ByteWidth = sizeof(XMFLOAT4);
	m_D3DDevice->CreateBuffer(&cbd, nullptr, m_ParameterBuffer.GetAddressOf());
	m_ImmediateContext->VSSetConstantBuffers(5, 1, m_ParameterBuffer.GetAddressOf());
	m_ImmediateContext->PSSetConstantBuffers(5, 1, m_ParameterBuffer.GetAddressOf());

}
