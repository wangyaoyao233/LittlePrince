#pragma once
#include <typeinfo>
#include "GameObject.h"
class Scene
{
public:
	/// <summary>
	/// Layer0 : Camera
	/// Layer1: 3D Objects
	/// Layer2: 2D Objects
	/// </summary>
	static constexpr int k_LayerNum = 3;
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Init() = 0;
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();

	template<typename T>
	T* AddGameObject(int layer)
	{
		T* gameObject = new T();
		m_GameObject[layer].push_back(gameObject);
		gameObject->Init();

		return gameObject;
	}

	template<typename T>
	T* GetGameObject()
	{
		for (int i = 0; i < k_LayerNum; i++)
		{
			for (GameObject* object : m_GameObject[i])
			{
				if (typeid(*object) == typeid(T))
				{
					return (T*)object;
				}
			}
		}
		return nullptr;
	}

	template<typename T>
	std::vector<T*> GetGameObjects()
	{
		std::vector<T*> result;
		for (int i = 0; i < k_LayerNum; i++)
		{
			for (GameObject* object : m_GameObject[i])
			{
				if (typeid(*object) == typeid(T))
				{
					result.push_back((T*)object);
				}
			}
		}
		return result;
	}

	GameObject* GetGameObjectWithTag(std::wstring tag)
	{
		for (int i = 0; i < k_LayerNum; i++)
		{
			for (GameObject* object : m_GameObject[i])
			{
				if (object->GetTag() == tag)
				{
					return object;
				}
			}
		}
		return nullptr;
	}


protected:
	std::list<GameObject*> m_GameObject[k_LayerNum];
};

