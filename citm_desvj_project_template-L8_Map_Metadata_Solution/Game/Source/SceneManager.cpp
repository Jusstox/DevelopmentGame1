#include "SceneManager.h"

#include "App.h"
#include "Level1.h"
#include "SceneIntro.h"
#include "Window.h"
#include "Log.h"


SceneManager::SceneManager()
{
	name.Create("scenemanager");

	sceneIntro = new SceneIntro();
	level1 = new Level1();

	scenes.Add(sceneIntro);
	scenes.Add(level1);

	sceneType = INTRO;

	currentScene = sceneIntro;
}

SceneManager::~SceneManager()
{

}

bool SceneManager::Awake(pugi::xml_node config)
{
	app->win->GetWindowSize(windowW, windowH);

	screenRect = { 0, 0, (int)windowW, (int)windowH };

	configScenes = config;
	bool ret = true;
	ListItem<Scene*>* item;
	Scene* pScene = NULL;

	for (item = scenes.start; item != NULL; item = item->next)
	{
		pScene = item->data;

		ret = item->data->Awake(config.child(item->data->name.GetString()));
	}
	return ret;
}

bool SceneManager::Start()
{
	sceneIntro->Init();
	sceneIntro->Start();
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool SceneManager::PreUpdate()
{
	return true;
}

bool SceneManager::Update(float dt)
{
	bool ret = true;
	ListItem<Scene*>* item;
	Scene* pScene = NULL;

	for (item = scenes.start; item != NULL; item = item->next)
	{
		pScene = item->data;

		if (pScene->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	if (fade) {
		MakeFade();
	}

	return ret;
}

bool SceneManager::PostUpdate()
{
	bool ret = true;
	ListItem<Scene*>* item;
	Scene* pScene = NULL;

	for (item = scenes.start; item != NULL; item = item->next)
	{
		pScene = item->data;

		if (pScene->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	if (fade) {
		float fadeRatio = (float)frameCount / (float)maxFadeFrames;
		LOG("%f", fadeRatio);
		// Render the black square with alpha on the screen
		SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
		SDL_RenderFillRect(app->render->renderer, &screenRect);
	}

	return ret;
}

bool SceneManager::CleanUp()
{
	return true;
}

bool SceneManager::LoadState(pugi::xml_node node)
{
	bool ret = true;

	switch (sceneType)
	{
	case INTRO:
		break;
	case LEVEL1:
		break;
	case NONE:
		break;
	default:
		break;
	}

	return ret;
}

void SceneManager::ChangeScane()
{
	switch (sceneType)
	{
	case INTRO:
		sceneIntro->CleanUp();
		sceneIntro->active = false;
		currentScene = level1;
		level1->Init();
		level1->Start();
		sceneType = LEVEL1;
		break;
	case LEVEL1:
		level1->CleanUp();
		level1->active = false;
		currentScene = sceneIntro;
		sceneIntro->Init();
		sceneIntro->Start();
		sceneType = INTRO;
		break;
	case NONE:
		break;
	default:
		break;
	}
}

void SceneManager::MakeFade()
{
	if (currentStep == Fade_Step::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			ChangeScane();
			currentStep = Fade_Step::FROM_BLACK;
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Fade_Step::NO;
			fade = false;
		}
	}
}

bool SceneManager::SaveState(pugi::xml_node node)
{
	bool ret = true;

	pugi::xml_node scene = node.append_child(name.GetString());
	scene.append_attribute("currentScene").set_value(sceneType);

	return ret;
}
