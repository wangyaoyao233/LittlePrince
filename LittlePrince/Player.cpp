/*system header*/
#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
/*tools header*/
#include "Input.h"
#include "AnimationModel.h"
#include "MeshField.h"
/*scenes header*/
/*gameobjects header*/
/*self header*/
#include "Player.h"

void Player::Init()
{
	m_Model = new AnimationModel();
	m_Model->Load("Asset/Model/Akai_Idle.fbx");
	m_Model->LoadAnimation("Asset/Model/Akai_Idle.fbx", "Idle");
	m_Model->LoadAnimation("Asset/Model/Akai_Run.fbx", "Run");
	m_Model->LoadAnimation("Asset/Model/Akai_Kick.fbx", "Kick");
	m_Model->LoadAnimation("Asset/Model/Akai_Jump.fbx", "Jump");

	m_Model->SetInitAnimation("Idle");

	// shader
	Renderer::CreateVertexShader(m_VertexShader.GetAddressOf(), m_VertexLayout.GetAddressOf(), L"HLSL/LightVS.cso");
	Renderer::CreatePixelShader(m_PixelShader.GetAddressOf(), L"HLSL/LightPS.cso");

	m_Position = XMFLOAT3(0.0f, 0.5f, 0.0f);
	m_RotationEuler = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scale = XMFLOAT3(0.02f, 0.02f, 0.02f);
	m_Rotation = XMQuaternionIdentity();

	m_Frame = 0;
}

void Player::Uninit()
{
	m_Model->UnLoad();
	delete m_Model;
}

void Player::Update()
{
	m_Frame++;

	XMVECTOR pos = XMLoadFloat3(&m_Position);
	XMFLOAT3 f = GetForward();
	XMVECTOR forward = XMLoadFloat3(&f);

	if (Input::GetKey(KeyCode::W))
	{
		pos -= forward * 0.1f;
		m_Model->Update("Run", m_Frame);
	}
	else if (Input::GetKey(KeyCode::K))
	{
		m_Model->Update("Kick", m_Frame);
	}
	else if (Input::GetKey(KeyCode::Space))
	{
		m_Model->Update("Jump", m_Frame);
	}
	else
	{
		m_Model->Update("Idle", m_Frame);
	}

	if (Input::GetKey(KeyCode::A))
	{
		XMVECTOR quat = XMQuaternionRotationNormal(XMVectorSet(0, 1, 0, 0), -0.05);
		m_Rotation = XMQuaternionMultiply(quat, m_Rotation);
	}
	if (Input::GetKey(KeyCode::D))
	{
		XMVECTOR quat = XMQuaternionRotationNormal(XMVectorSet(0, 1, 0, 0), 0.05);
		m_Rotation = XMQuaternionMultiply(quat, m_Rotation);
	}

	// Store
	XMStoreFloat3(&m_Position, pos);


	auto meshField = Manager::GetScene()->GetGameObject<MeshField>();
	m_Position.y = meshField->GetHeight(m_Position);


}

void Player::Draw()
{
	//Set InputLayout
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout.Get());
	//Set Shader
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	//W = SRT
	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	/*rot = XMMatrixRotationRollPitchYaw(m_RotationEuler.z, m_RotationEuler.x, m_RotationEuler.y);*/
	rot = XMMatrixRotationQuaternion(m_Rotation);
	XMQuaternionNormalize(m_Rotation);
	trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot * trans;
	Renderer::SetWorldMatrix(world);

	m_Model->Draw();
}