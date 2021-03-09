#pragma once
#include "GameObject.h"

class AnimationModel;
class Player : public GameObject
{
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

private:
	ComPtr<ID3D11VertexShader> m_VertexShader;
	ComPtr<ID3D11PixelShader> m_PixelShader;
	ComPtr<ID3D11InputLayout> m_VertexLayout;

	AnimationModel* m_Model;
	unsigned int m_Frame;
};

