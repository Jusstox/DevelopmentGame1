#ifndef __MENU__
#define __MENU__
#include "Scene.h"

struct SDL_Texture;

class Menu :  public Scene
{
public:

	Menu();

	// Destructor
	virtual ~Menu();

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

private:
	int fx;
	SDL_Texture* img;
	float textPosX, textPosY = 0;
	uint texW, texH;
	pugi::xml_node config;
};

#endif //__MENU__