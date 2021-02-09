/*system header*/
#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
/*tools header*/
/*scenes header*/
/*gameobjects header*/
/*self header*/
#include "Scene.h"

void Scene::Uninit()
{
	for (int i = 0; i < k_LayerNum; i++)
	{
		for (GameObject* object : m_GameObject[i])
		{
			object->Uninit();
			delete object;
		}
		m_GameObject[i].clear();
	}
}

void Scene::Update()
{
	for (int i = 0; i < k_LayerNum; i++)
	{
		for (GameObject* object : m_GameObject[i])
		{
			object->Update();
		}

		m_GameObject[i].remove_if(
			[](GameObject* object)
			{
				return object->Destroy();
			});
	}
}

void Scene::Draw()
{
	for (int i = 0; i < k_LayerNum; i++)
	{
		for (auto object : m_GameObject[i])
		{
			object->Draw();
		}
	}
}
