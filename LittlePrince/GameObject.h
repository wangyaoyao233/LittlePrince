#pragma once

#include "Main.h"


class GameObject
{
public:
	GameObject() {}
	virtual ~GameObject() {}

	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool Destroy()
	{
		if (m_Destroy)
		{
			Uninit();
			delete this;
			return true;
		}
		else
		{
			return false;
		}
	}

	/// Getter
	XMFLOAT3 GetPosition() const { return m_Position; }
	XMFLOAT3 GetScale() const { return m_Scale; }
	XMFLOAT3 GetRotationEuler() const { return m_RotationEuler; }
	XMVECTOR GetRotation() const { return m_Rotation; }
	int GetPriority() const { return m_Priority; }
	std::wstring GetTag() const { return m_Tag; }
	XMFLOAT3 GetForward() const
	{
		XMFLOAT4X4 rot;
		XMStoreFloat4x4(&rot, XMMatrixRotationQuaternion(m_Rotation));
		//XMStoreFloat4x4(&rot, XMMatrixRotationRollPitchYaw(m_RotationEuler.z, m_RotationEuler.x, m_RotationEuler.y));

		XMFLOAT3 forward;
		forward.x = rot._31;
		forward.y = rot._32;
		forward.z = rot._33;

		return forward;
	}
	XMFLOAT3 GetUp() const
	{
		XMFLOAT4X4 rot;
		XMStoreFloat4x4(&rot, XMMatrixRotationQuaternion(m_Rotation));
		//XMStoreFloat4x4(&rot, XMMatrixRotationRollPitchYaw(m_RotationEuler.z, m_RotationEuler.x, m_RotationEuler.y));

		XMFLOAT3 up;
		up.x = rot._21;
		up.y = rot._22;
		up.z = rot._23;

		return up;
	}
	XMFLOAT3 GetRight() const
	{
		XMFLOAT4X4 rot;
		XMStoreFloat4x4(&rot, XMMatrixRotationQuaternion(m_Rotation));
		//XMStoreFloat4x4(&rot, XMMatrixRotationRollPitchYaw(m_RotationEuler.z, m_RotationEuler.x, m_RotationEuler.y));

		XMFLOAT3 right;
		right.x = rot._11;
		right.y = rot._12;
		right.z = rot._13;

		return right;
	}

	/// Setter
	void SetPosition(XMFLOAT3 value) { m_Position = value; }
	void SetScale(XMFLOAT3 value) { m_Scale = value; }
	void SetRoationEuler(XMFLOAT3 value) { m_RotationEuler = value; }
	void SetRotation(XMVECTOR value) { m_Rotation = value; }
	void SetTag(std::wstring value) { m_Tag = value; }
	virtual void SetDestroy() { m_Destroy = true; }

protected:
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Scale;
	XMFLOAT3 m_RotationEuler;
	XMVECTOR m_Rotation;//Quaternion : Identity

	bool m_Destroy = false;

	int m_Priority = 0;
	std::wstring m_Tag;
};