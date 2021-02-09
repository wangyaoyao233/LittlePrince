/*system header*/
#include "Main.h"
#include "Renderer.h"
/*tools header*/
/*scenes header*/
#include "Game.h"
/*gameobjects header*/
/*self header*/
#include "Manager.h"

Scene* Manager::m_Scene = nullptr;

void Manager::Init()
{
	Renderer::Init();

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

	m_Scene->Update();
}

void Manager::Draw()
{
	Renderer::Begin();

	m_Scene->Draw();

	Renderer::End();
}
