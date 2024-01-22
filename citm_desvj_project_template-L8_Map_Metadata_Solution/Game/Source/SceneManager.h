#ifndef __SCENE_MANAGER__
#define __SCENE_MANAGER__

#include "Module.h"
#include "Scene.h"

#include "List.h"

#include "SDL/include/SDL_rect.h"

class Level1;
class Level2;
class SceneIntro;
class Menu;
class Settings;

enum Fade_Step
{
	NO,
	TO_BLACK,
	FROM_BLACK
};

enum SceneType
{
	INTRO,
	MENU,
	LEVEL1,
	LEVEL2,
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

	void OpenSettings();

	void CloseSettings();

	Scene* currentScene;
	Scene* newScene = nullptr;

	void ChangeScane();

	Level1* level1;
	Level2* level2;
	SceneIntro* sceneIntro;
	Menu* menu;
	Settings* settings;

	uint windowW, windowH;

	bool fade = false;

	void MakeFade();

	Fade_Step currentStep = Fade_Step::NO;

	Uint32 maxFadeFrames = 75;
	Uint32 maxFadeFramesBack = 0;

private:

	SDL_Rect screenRect;

	List<Scene*> scenes;

	SceneType sceneType;

	pugi::xml_node configScenes;

	// A frame count system to handle the fade time and ratio
	Uint32 frameCount = 0;
};

#endif	// __SCENE_MANAGER__