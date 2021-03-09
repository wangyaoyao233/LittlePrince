/*system header*/
#include "Main.h"
#include "Renderer.h"
/*tools header*/
#include "Input.h"
/*scenes header*/
#include "Game.h"
/*gameobjects header*/
/*self header*/
#include "Manager.h"

Scene* Manager::m_Scene = nullptr;

void Manager::Init()
{
	Renderer::Init();
	Input::Init();

	SetScene<Game>();
}

void Manager::Uninit()
{
	m_Scene->Uninit();
	delete m_Scene;
	m_Scene = nullptr;



	Renderer::Uninit();
}

void Manager::Update()
{
	Input::PreUpdate();

	m_Scene->Update();

	Input::PostUpdate();
}

void Manager::Draw()
{
	Renderer::Begin();

	m_Scene->Draw();

	Renderer::End();
}
