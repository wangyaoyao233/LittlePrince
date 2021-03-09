/*system header*/
#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
/*tools header*/
#include "Light.h"
/*scenes header*/
/*gameobjects header*/
#include "Polygon.h"
#include "Camera.h"
#include "Player.h"
#include "MeshField.h"
/*self header*/
#include "Game.h"

void Game::Init()
{
	Light::InitResource();

	AddGameObject<Camera>(0);
	AddGameObject<Player>(1);
	AddGameObject<MeshField>(1);

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
	Light::UpdateLightConstantBuffer();
	Scene::Draw();
}