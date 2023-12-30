#include "Menu.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "SceneManager.h"
#include "Window.h"
#include "Audio.h"

Menu::Menu() : Scene()
{
	name.Create("menu");
}

Menu::~Menu()
{
}

bool Menu::Awake(pugi::xml_node config)
{
	this->config = config;
	return true;
}

bool Menu::Start()
{
	img = app->tex->Load(config.attribute("texturePath").as_string());
	app->audio->PlayMusic(config.attribute("musicpath").as_string(), 1.5);
	app->tex->GetSize(img, texW, texH);
	app->render->camera.x = 0;
	app->render->camera.y = 0;
	app->win->GetWindowSize(windowW, windowH);
	return true;
}

bool Menu::PreUpdate()
{
	return true;
}

bool Menu::Update(float dt)
{
	return true;
}

bool Menu::PostUpdate()
{
	app->render->DrawTexture(img, windowW / 2 - texW / 2, windowH / 2 - texH / 2, NULL);
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		app->sceneManager->fade = true;
		app->sceneManager->currentStep = TO_BLACK;
		app->sceneManager->maxFadeFrames = 100;
	}
	return true;
}

bool Menu::CleanUp()
{
	app->tex->UnLoad(img);
	return true;
}
