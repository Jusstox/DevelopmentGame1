#include "SceneManager.h"

#include "App.h"
#include "Level1.h"
#include "Map.h"


SceneManager::SceneManager()
{
	name.Create("scenemanager");

	level1 = new Level1();

	scenes.Add(level1);

	sceneType = LEVEL1;

	currentScene = level1;
}

SceneManager::~SceneManager()
{

}

bool SceneManager::Awake(pugi::xml_node config)
{
	level1->Init();
	level1->Awake(config.child(level1->name.GetString()));
	level1->Start();
	return true;
}

bool SceneManager::Start()
{
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

bool SceneManager::SaveState(pugi::xml_node node)
{
	bool ret = true;

	pugi::xml_node scene = node.append_child(name.GetString());
	scene.append_attribute("currentScene").set_value(sceneType);

	return ret;
}
