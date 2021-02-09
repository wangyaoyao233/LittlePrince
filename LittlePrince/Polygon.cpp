/*system header*/
#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
/*tools header*/

/*scenes header*/

/*gameobjects header*/

/*self header*/
#include "Polygon.h"

void CPolygon::Init()
{
	VERTEX_3D vertex[4];

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(400.0f, 0.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(0.0f, 400.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(400.0f, 400.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, m_VertexBuffer.GetAddressOf());

	Renderer::CreateVertexShader(m_VertexShader.GetAddressOf(), m_VertexLayout.GetAddressOf(), L"HLSL/2DVS.cso");
	Renderer::CreatePixelShader(m_PixelShader.GetAddressOf(), L"HLSL/2DPS.cso");

	CreateWICTextureFromFile(Renderer::GetDevice().Get(), L"./Asset/Texture/Fire001.bmp", nullptr, m_Texture.GetAddressOf());
	assert(m_Texture);
}

void CPolygon::Uninit()
{
}

void CPolygon::Update()
{
}

void CPolygon::Draw()
{
	// Set InputLayout
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout.Get());
	// Set Shader
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader.Get(), nullptr, 0);
	// Set WVP
	Renderer::SetWorldViewProjection2D();

	// Set VertexBuffer
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);

	// Set ShaderResource
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, m_Texture.GetAddressOf());

	// Set PrimitiveTopology
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// Draw
	Renderer::GetDeviceContext()->Draw(4, 0);
}
