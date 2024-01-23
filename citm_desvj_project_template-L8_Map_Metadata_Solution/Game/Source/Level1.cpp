#include "Level1.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Item.h"
#include "Physics.h"
#include "Enemy.h"
#include "SceneManager.h"
#include "GuiManager.h"
#include "Settings.h"
#include "Level2.h"

#include "Defs.h"
#include "Log.h"

Level1::Level1() : Scene()
{
	name.Create("level1");
}

Level1::~Level1()
{
}

bool Level1::Awake(pugi::xml_node config)
{
	LOG("Loading Level1");
	bool ret = true;

	sceneconfig = config;

	//Get the map name from the config file and assigns the value in the module
	app->map->name = config.attribute("name").as_string();
	app->map->path = config.attribute("path").as_string();

	// iterate all items in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	/*for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}*/

	//Load enemies
	pugi::xml_node enemieNode = config.child("enemies");
	//FlyEnemy
	for (pugi::xml_node flyenemieNode = enemieNode.child("flyenemy"); flyenemieNode; flyenemieNode = flyenemieNode.next_sibling("flyenemy"))
	{
		Enemy* EnemyFly = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMYFLY);
		EnemyFly->parameters = flyenemieNode;
		EnemyFly->animconfig = enemieNode;
		EnemyFly->lvl = 1;
	}

	//WalkerEnemy
	for (pugi::xml_node walkenemieNode = enemieNode.child("walkenemy"); walkenemieNode; walkenemieNode = walkenemieNode.next_sibling("walkenemy"))
	{
		Enemy* EnemySlime = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMYSLIME);
		EnemySlime->parameters = walkenemieNode;
		EnemySlime->animconfig = enemieNode;
		EnemySlime->lvl = 1;
	}

	//L03: DONE 3b: Instantiate the player using the entity manager
	//L04 DONE 7: Get player paremeters
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	//Assigns the XML node to a member in player
	player->config = config.child("player");

	return ret;
}

bool Level1::Start()
{
	if (player->lvl == 2 || reset) {
		app->sceneManager->level2->CleanUp();
		app->map->name = sceneconfig.attribute("name").as_string();
		app->map->path = sceneconfig.attribute("path").as_string();
		if (player->lvl == 1 || player->lvl == 2) {
			player->respawn();
			app->entityManager->respawnEntities(1);
		}
		player->lvl = 1;
		reset = false;
	}
	app->map->InitMap();
	app->entityManager->Lvl1EntitiesActive();
	app->entityManager->Start();
	app->audio->PlayMusic(sceneconfig.attribute("musicpath").as_string(),1.5);
	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	app->render->camera.y = ((player->position.y - 26 / 2) - windowH / 2) * -1;
	app->render->camera.x = ((player->position.x - 40 / 2) - (windowW / 2)) * -1;

	if (app->render->camera.x >= 0) {
		app->render->camera.x = 0;
	}
	if (app->render->camera.y >= 0) {
		app->render->camera.y = 0;
	}
	settings = false;
	return true;
}

bool Level1::PreUpdate()
{
	return true;
}

bool Level1::Update(float dt)
{
	if (lockCamera) {
		cameraLimit();
	}
	else {
		//L02 DONE 3: Make the camera movement independent of framerate
		float camSpeed = 1;

		if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
			app->render->camera.y -= (int)ceil(camSpeed * dt);
		}

		if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
			app->render->camera.y += (int)ceil(camSpeed * dt);
		}

		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
			app->render->camera.x -= (int)ceil(camSpeed * dt);
		}

		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
			app->render->camera.x += (int)ceil(camSpeed * dt);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		lockCamera = !lockCamera;
	}
	// L14: TODO 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveRequest();
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadRequest();

	if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {
		app->sceneManager->fade = true;
		app->sceneManager->newScene = (Scene*)app->sceneManager->level2;
		app->sceneManager->currentStep = TO_BLACK;
		app->sceneManager->maxFadeFrames = 200;
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && !app->sceneManager->settings->menuSetings && settings == false) {
		app->sceneManager->OpenSettings();
		settings = true;
	}

	if (quit) {
		return false;
	}

	return true;
}

bool Level1::PostUpdate()
{
	bool ret = true;

	//if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		//ret = false;

	return ret;
}

bool Level1::CleanUp()
{
	LOG("Freeing scene");
	app->map->CleanUp();
	app->map->active = false;
	app->entityManager->ActiveNone();
	return true;
}

void Level1::cameraLimit()
{
	int limitCamXend = (app->map->getMapWidth() - windowW) * -1;
	int limitCamXbeg = 0;

	app->render->camera.y = ((player->position.y - 26 / 2) - windowH / 2) * -1;
	app->render->camera.x = ((player->position.x - 40 / 2) - (windowW / 2)) * -1;

	if (app->render->camera.x > limitCamXbeg) {
		app->render->camera.x = limitCamXbeg;
	}
	else if (app->render->camera.x < limitCamXend) {
		app->render->camera.x = limitCamXend;
	}

	int limitCamYend = (app->map->getMapHeght() - windowH) * -1;
	int limitCamYbeg = 0;

	if (app->render->camera.y > limitCamYbeg) {
		app->render->camera.y = limitCamYbeg;
	}
	else if (app->render->camera.y < limitCamYend) {
		app->render->camera.y = limitCamYend;
	}
}
