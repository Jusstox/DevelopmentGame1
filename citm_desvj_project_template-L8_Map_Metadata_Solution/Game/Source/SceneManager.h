#ifndef __SCENE_MANAGER__
#define __SCENE_MANAGER__

#include "Module.h"
#include "Scene.h"
#include "List.h"

class Level1;

enum SceneType
{
	INTRO,
	LEVEL1,
	NONE
};

class SceneManager : public Module
{
public:

	SceneManager();

	// Destructor
	virtual ~SceneManager();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool LoadState(pugi::xml_node node);

	bool SaveState(pugi::xml_node node);

	Scene* currentScene;

private:

	List<Scene*> scenes;

	Level1* level1;

	SceneType sceneType;

	pugi::xml_node configScenes;
};

#endif	// __SCENE_MANAGER__