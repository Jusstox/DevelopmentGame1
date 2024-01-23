#ifndef __SCENE_H__
#define __SCENE_H__

#include "SString.h"

#include "PugiXml/src/pugixml.hpp"

class Player;
class GuiControl;

class Scene
{
public:

	Scene() : active(false)
	{}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node config)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual Player* GetPlayer() {
		return player;
	}

	virtual bool OnGuiMouseClickEvent(GuiControl* control)
	{
		return true;
	}

	uint windowW, windowH;

public:

	pugi::xml_node sceneconfig;

	SString name;
	bool active;
	bool settings;
	Player* player = nullptr;
	bool quit = false;
};

#endif // __SCENE_H__
