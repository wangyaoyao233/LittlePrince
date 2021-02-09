/*system header*/
#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
/*tools header*/
/*scenes header*/
/*gameobjects header*/
#include "Polygon.h"
/*self header*/
#include "Game.h"

void Game::Init()
{
	AddGameObject<CPolygon>(2);
}

void Game::Uninit()
{
	Scene::Uninit();
}

void Game::Update()
{
	Scene::Update();
}

void Game::Draw()
{
	Scene::Draw();
}