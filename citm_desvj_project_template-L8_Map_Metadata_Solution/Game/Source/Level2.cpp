#include "Level2.h"

#include "App.h"
#include "SceneManager.h"
#include "Enemy.h"
#include "Settings.h"
#include "Level1.h"
#include "Map.h"
#include "Audio.h"
#include "Window.h"

Level2::Level2() : Scene()
{
	name.Create("level2");
}

Level2::~Level2()
{
}

bool Level2::Awake(pugi::xml_node config)
{
	sceneconfig = config;
	player = app->sceneManager->level1->GetPlayer();

	pugi::xml_node enemieNode = config.child("enemies");
	for (pugi::xml_node flyenemieNode = enemieNode.child("flyenemy"); flyenemieNode; flyenemieNode = flyenemieNode.next_sibling("flyenemy"))
	{
		Enemy* EnemyFly = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMYFLY);
		EnemyFly->parameters = flyenemieNode;
		EnemyFly->animconfig = enemieNode;
		EnemyFly->lvl = 2;
	}
	return true;
}

bool Level2::Start()
{
	if (player->lvl == 1) {
		if (player->lvl == 1) {
			app->sceneManager->level1->CleanUp();
		}
		if (player->lvl == 1 || player->lvl == 2) {
			player->respawn();
			app->entityManager->respawnEntities(2);
		}
		player->lvl = 2;
	}
	app->map->name = sceneconfig.attribute("name").as_string();
	app->map->path = sceneconfig.attribute("path").as_string();
	app->map->InitMap();
	player->lvl = 2;
	app->entityManager->Lvl2EntitiesActive();
	app->entityManager->Start();
	player->respawn();
	app->audio->PlayMusic(sceneconfig.attribute("musicpath").as_string(), 1.5);

	app->win->GetWindowSize(windowW, windowH);

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

bool Level2::PreUpdate()
{
	return true;
}

bool Level2::Update(float dt)
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


	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && !app->sceneManager->settings->menuSetings && settings == false) {
		app->sceneManager->OpenSettings();
		settings = true;
	}

	if (quit) {
		return false;
	}

	return true;
}

bool Level2::PostUpdate()
{
	return true;
}

bool Level2::CleanUp()
{
	app->map->CleanUp();
	app->map->active = false;
	app->entityManager->ActiveNone();
	return true;
}

void Level2::cameraLimit()
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
