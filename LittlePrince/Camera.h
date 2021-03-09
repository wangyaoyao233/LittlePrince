#pragma once
#include "GameObject.h"
class Camera : public GameObject
{
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	XMFLOAT4X4 GetView() { return m_View; }
	XMFLOAT4X4 GetProjection() { return m_Projection; }
private:
	XMFLOAT3 m_Target;

	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Projection;
};

