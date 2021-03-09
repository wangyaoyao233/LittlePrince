/*system header*/
#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
/*tools header*/
/*scenes header*/
/*gameobjects header*/
#include "Player.h"
/*self header*/
#include "Camera.h"

void Camera::Init()
{
	m_Position = XMFLOAT3(0.0f, 3.0f, -10.0f);
	m_Target = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Camera::Uninit()
{
}

void Camera::Update()
{
	//1.target设为player
	auto player = Manager::GetScene()->GetGameObject<Player>();
	m_Target = player->GetPosition();

	//2. position设为player身后
	XMFLOAT3 playerPosition = player->GetPosition();
	XMFLOAT3 playerFoward = player->GetForward();
	auto position = XMLoadFloat3(&playerPosition) + 15 * XMLoadFloat3(&playerFoward) + 6 * g_XMIdentityR1;
	XMStoreFloat3(&m_Position, position);
}

void Camera::Draw()
{
	//Set View Matrix
	XMMATRIX viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&m_Target), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&m_View, viewMatrix);
	Renderer::SetViewMatrix(viewMatrix);

	//Set Projection Matrix
	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Projection, projectionMatrix);
	Renderer::SetProjectionMatrix(projectionMatrix);

	Renderer::SetCameraPosition(m_Position);
}