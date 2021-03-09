#pragma once
#include "GameObject.h"
class MeshField :public GameObject
{
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	float GetHeight(XMFLOAT3 position);

private:
	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;
	ComPtr<ID3D11ShaderResourceView> m_Texture;

	ComPtr<ID3D11VertexShader> m_VertexShader;
	ComPtr<ID3D11PixelShader> m_PixelShader;
	ComPtr<ID3D11InputLayout> m_VertexLayout;

	VERTEX_3D m_Vertex[21][21];
};

