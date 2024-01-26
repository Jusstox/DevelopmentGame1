#include "SceneIntro.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "SceneManager.h"
#include "Window.h"
#include "Audio.h"

SceneIntro::SceneIntro() : Scene()
{
	name.Create("sceneintro");
}

SceneIntro::~SceneIntro()
{
}

bool SceneIntro::Awake(pugi::xml_node config)
{
	this->config = config;
	return true;
}

bool SceneIntro::Start()
{
	img = app->tex->Load(config.attribute("texturePath").as_string());
	if (fx == -1) {
		fx = app->audio->LoadFx(config.attribute("fx").as_string());
	}
	app->tex->GetSize(img, texW, texH);
	app->render->camera.x = 0;
	app->render->camera.y = 0;
	app->win->GetWindowSize(windowW, windowH);
	app->audio->PlayFx(fx);
	return true;
}

bool SceneIntro::PreUpdate()
{
	return true;
}

bool SceneIntro::Update(float dt)
{
	return true;
}

bool SceneIntro::PostUpdate()
{
	OPTICK_EVENT();

	app->render->DrawTexture(img, windowW/2 - texW/2, windowH / 2 - texH / 2,NULL);
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		app->sceneManager->fade = true;
		app->sceneManager->newScene = (Scene*)app->sceneManager->menu;
		app->sceneManager->currentStep = TO_BLACK;
		app->sceneManager->maxFadeFrames = 100;
	//	app->sceneManager->maxFadeFramesBack = 200;
	}
	return true;
}

bool SceneIntro::CleanUp()
{
	app->tex->UnLoad(img);
	return true;
}
