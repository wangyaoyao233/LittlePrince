/*system header*/
#include "main.h"
#include "manager.h"
#include "renderer.h"
/*tools header*/

/*scenes header*/

/*gameobjects header*/

/*self header*/
#include "MeshField.h"

float g_FieldHeight[21][21] =
{
	{0.0f,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,3.0f,9.0f,10.0f,10.0f},
	{0.0f,1.0f,2.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,3.0f,9.0f,10.0f,10.0f},
	{0.0f,1.0f,2.0f,1.0f,3.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,3.0f,8.0f,9.0f,9.0f},
	{0.0f,1.0f,5.0f,5.0f,7.0f,3.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,0.0f,3.0f,7.0f,0.0f},
	{0.0f,1.0f,5.0f,6.0f,7.0f,2.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,0.0f,3.0f,0.0f},
	{0.0f,4.0f,5.0f,5.0f,4.0f,2.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,4.0f,5.0f,4.0f,3.0f,2.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,1.0f,2.0f,4.0f,2.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f},
	{0.0f,1.0f,2.0f,3.0f,4.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,2.0f,1.0f,0.0f},
	{0.0f,1.0f,2.0f,0.0f,2.0f,0.0f,0.0f,0.0f,0.0f,2.0f,3.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,2.0f,3.0f,2.0f,0.0f},
	{0.0f,0.0f,2.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,3.0f,2.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,2.0f,4.0f,3.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,3.0f,3.0f,4.0f,3.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,4.0f,0.0f,0.0f,1.0f,2.0f,1.0f,0.0f,0.0f,0.0f,1.0f,2.0f,2.0f,5.0f,4.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,4.0f,2.0f,0.0f,1.0f,2.0f,2.0f,2.0f,0.0f,0.0f,0.0f,0.0f,4.0f,5.0f,4.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,3.0f,4.0f,3.0f,1.0f,0.0f,0.0f,1.0f,2.0f,4.0f,5.0f,4.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,3.0f,0.0f,0.0f,2.0f,2.0f,1.0f,0.0f,0.0f,0.0f,0.0f,3.0f,2.0f,3.0f,3.0f,0.0f},
	{0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,1.0f,2.0f,0.0f},
	{0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,2.0f,0.0f},
	{0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f},
	{0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,1.0f,2.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
};


void MeshField::Init()
{
	//VertexBuffer
	{
		float width = 5.0f;
		for (int x = 0; x <= 20; x++)
		{
			for (int z = 0; z <= 20; z++)
			{
				float y = g_FieldHeight[z][x];
				//float y = sinf(x * 0.5f) * sinf(z * 0.5f) * 3.0f;
				m_Vertex[x][z].Position = XMFLOAT3(width * (x - 10), y, -width * (z - 10));
				m_Vertex[x][z].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

				//根据高度设置颜色
				float f = y / 10.0f + 0.5f;
				m_Vertex[x][z].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				//m_Vertex[x][z].Diffuse = XMFLOAT4(1.0f * f, 1.0f * f, 1.0f * f, 1.0f);

				m_Vertex[x][z].TexCoord = XMFLOAT2(x * 0.5f, z * 0.5f);
			}
		}
		for (int x = 1; x <= 19; x++)
		{
			for (int z = 1; z <= 19; z++)
			{
				XMVECTOR vx, vz, vn;
				vx = XMLoadFloat3(&m_Vertex[x + 1][z].Position) - XMLoadFloat3(&m_Vertex[x - 1][z].Position);
				vz = XMLoadFloat3(&m_Vertex[x][z - 1].Position) - XMLoadFloat3(&m_Vertex[x][z + 1].Position);

				vn = XMVector3Normalize(XMVector3Cross(vz, vx));
				XMStoreFloat3(&m_Vertex[x][z].Normal, vn);			
			}
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * 21 * 21;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_Vertex;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, m_VertexBuffer.GetAddressOf());
	}
	//IndexBuffer
	{
		unsigned int index[(22 * 2) * 20 - 2];

		int i = 0;
		for (int x = 0; x < 20; x++)
		{
			//一般点的处理方法
			for (int z = 0; z < 21; z++)
			{
				index[i] = x * 21 + z;
				i++;

				index[i] = (x + 1) * 21 + z;
				i++;
			}
			//特殊点的处理方法
			if (x == 19)
			{
				break;
			}
			index[i] = (x + 1) * 21 + 20;
			i++;

			index[i] = (x + 1) * 21;
			i++;
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * ((22 * 2) * 20 - 2);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, m_IndexBuffer.GetAddressOf());
	}

	// shader
	Renderer::CreateVertexShader(m_VertexShader.GetAddressOf(), m_VertexLayout.GetAddressOf(), L"HLSL/LightVS.cso");
	Renderer::CreatePixelShader(m_PixelShader.GetAddressOf(), L"HLSL/LightPS.cso");


	//D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(), L"asset/texture/green_lowpoly.png", NULL, NULL, m_Texture.GetAddressOf(), NULL);
	CreateDDSTextureFromFile(Renderer::GetDevice().Get(), L"Asset/Texture/green_lowpoly.dds", nullptr, m_Texture.GetAddressOf());
	//CreateWICTextureFromFile(Renderer::GetDevice().Get(), L"./Asset/Texture/green_lowpoly.png", nullptr, m_Texture.GetAddressOf());
	assert(m_Texture);

	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_RotationEuler = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

void MeshField::Uninit()
{
}

void MeshField::Update()
{
}

void MeshField::Draw()
{

	//Set InputLayout
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout.Get());
	//Set Shader
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	//W = SRT
	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	rot = XMMatrixRotationRollPitchYaw(m_RotationEuler.z, m_RotationEuler.x, m_RotationEuler.y);
	trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

	world = scale * rot * trans;

	Renderer::SetWorldMatrix(world);

	//set vertex buffer
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);

	//Set Index Buffer
	Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


	///set material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	material.Shininess = 60.0f;
	Renderer::SetMaterial(material);

	//set texture(shader resources)
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, m_Texture.GetAddressOf());

	//set primitive topology
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//draw
	Renderer::GetDeviceContext()->DrawIndexed((22 * 2) * 20 - 2, 0, 0);
}

float MeshField::GetHeight(XMFLOAT3 position)
{
	int x, z;
	
	x = position.x / 5.0f + 10.0f;
	z = position.z / -5.0f + 10.0f;

	XMFLOAT3 pos0, pos1, pos2, pos3;

	pos0 = m_Vertex[x][z].Position;
	pos1 = m_Vertex[x + 1][z].Position;
	pos2 = m_Vertex[x][z + 1].Position;
	pos3 = m_Vertex[x + 1][z + 1].Position;

	XMVECTOR v12, v1p, c;
	v12 = XMLoadFloat3(&pos2) - XMLoadFloat3(&pos1);
	v1p = XMLoadFloat3(&position) - XMLoadFloat3(& pos1);

	c = XMVector3Cross(v12, v1p);

	XMFLOAT3 tmp;
	XMVECTOR n;

	XMStoreFloat3(&tmp, c);
	if (tmp.y > 0.0f)
	{
		XMVECTOR v10;
		v10 = XMLoadFloat3(&pos0) - XMLoadFloat3(&pos1);
		n = XMVector3Cross(v10, v12);
	}
	else
	{
		XMVECTOR v13;
		v13 = XMLoadFloat3(&pos3) - XMLoadFloat3(&pos1);
		n = XMVector3Cross(v12, v13);
	}

	XMStoreFloat3(&tmp, n);
	float py;
	//n * v1p = 0 ==> n.x(v1p.x) + n.y(v1p.y) + n.z(v1p.z) = 0 ==>
	py = -((position.x - pos1.x) * tmp.x + (position.z - pos1.z) * tmp.z) / tmp.y + pos1.y;

	return py;
}
