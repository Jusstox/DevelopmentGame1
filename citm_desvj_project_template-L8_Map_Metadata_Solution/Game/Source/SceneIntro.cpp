#include "SceneIntro.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "SceneManager.h"

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
	app->render->camera.x = 0;
	app->render->camera.y = 0;
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
	app->render->DrawTexture(img,0,0,NULL);
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		app->sceneManager->ChangeScane();
	}
	return true;
}

bool SceneIntro::CleanUp()
{
	app->tex->UnLoad(img);
	return true;
}
