#ifndef __SETTINGS__
#define __SETTINGS__

#include "Scene.h"
#include "GuiControl.h"
#include "GuiControlButton.h"
#include "GuiSlider.h"
#include "GuiCheckBox.h"

class Settings :  public Scene
{
public:

	Settings();

	// Destructor
	virtual ~Settings();

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

	bool OnGuiMouseClickEvent(GuiControl* control);

	uint windowW, windowH;

private:
	SDL_Rect quat;
	pugi::xml_node config;
	bool quit = false;
	bool fullscreen;
	// L15: TODO 2: Declare a GUI Control Button 
	GuiSlider* gcMSlider;
	GuiSlider* gcFSlider;
	GuiCheckBox* checkBox;
};

#endif //__SETTINGS__
