#pragma once

// 頂点構造体
struct VERTEX_3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Diffuse;
	XMFLOAT2 TexCoord;
};

// マテリアル構造体
struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	float		Dummy[3];//16byte境界用
};

// マテリアル構造体
struct DX11_MODEL_MATERIAL
{
	MATERIAL		Material;
	class CTexture* Texture;
};

// 描画サブセット構造体
struct DX11_SUBSET
{
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	DX11_MODEL_MATERIAL	Material;
};

class Renderer
{
public:
	static bool Init();
	static void Uninit();
	static void Begin();
	static void End();
	static void OnResize();

	static ComPtr<ID3D11Device> GetDevice() { return m_D3DDevice; }
	static ComPtr<ID3D11DeviceContext> GetDeviceContext() { return m_ImmediateContext; }

	static void SetWorldViewProjection2D();
	static void SetWorldMatrix(XMMATRIX WorldMatrix);
	static void SetViewMatrix(XMMATRIX ViewMatrix);
	static void SetProjectionMatrix(XMMATRIX ProjectionMatrix);
	static void SetCameraPosition(XMFLOAT3 CameraPos);
	static void SetParameter(XMFLOAT4 Parameter);
	static void SetMaterial(MATERIAL& Material);

	static void CreateVertexShader(ID3D11VertexShader** vertexShader, ID3D11InputLayout** vertexLayout, std::wstring fileName);
	static void CreatePixelShader(ID3D11PixelShader** pixelShader, std::wstring fileName);

private:
	static void CreateConstantBuffer();

private:
	static ComPtr<ID3D11Device> m_D3DDevice;// D3D11设备
	static ComPtr<ID3D11Device> m_D3DDevice1;// D3D11.1设备
	static ComPtr<ID3D11DeviceContext> m_ImmediateContext;
	static ComPtr<ID3D11DeviceContext> m_ImmediateContext1;
	static ComPtr<IDXGISwapChain> m_pSwapChain;
	static ComPtr<IDXGISwapChain1> m_pSwapChain1;
	static ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	static ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
	static ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;

	static ComPtr<ID3D11SamplerState> m_pSamplerState;// 采样器
	static UINT m_4xMsaaQuality;

	// constant buffer
	static ComPtr<ID3D11Buffer> m_WorldBuffer;
	static ComPtr<ID3D11Buffer> m_ViewBuffer;
	static ComPtr<ID3D11Buffer> m_ProjectionBuffer;
	static ComPtr<ID3D11Buffer> m_MaterialBuffer;
	static ComPtr<ID3D11Buffer> m_CameraBuffer;
	static ComPtr<ID3D11Buffer> m_ParameterBuffer;
};

