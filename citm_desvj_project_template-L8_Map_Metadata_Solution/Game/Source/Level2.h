#pragma once
#include "Scene.h"
#include "Player.h"

class Level2 :  public Scene
{
public:
	Level2();

	// Destructor
	virtual ~Level2();

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
	uint windowW, windowH;

	Player* GetPlayer() {
		return player;
	}

	void cameraLimit();

	bool lockCamera = true;
	//L03: DONE 3b: Declare a Player attribute
	Player* player;
};

