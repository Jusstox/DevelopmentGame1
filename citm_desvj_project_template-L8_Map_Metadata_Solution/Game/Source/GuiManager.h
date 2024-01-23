#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "GuiControl.h"

#include "List.h"

class GuiManager : public Module
{
public:

	// Constructor
	GuiManager();

	// Destructor
	virtual ~GuiManager();

	// Called before the first frame
	 bool Start();

	 // Called each loop iteration
	 bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	bool DeleteGuiControl(GuiControl* guiControl);

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Scene* observer, SDL_Rect sliderBounds = { 0,0,0,0 });

	GuiControl* GetFromID(int id);

	void ActiveAllGui();

	void DesactvieAllGui();

public:

	List<GuiControl*> guiControlsList;
	SDL_Texture* texture;

	SDL_Rect quat;
	uint windowW, windowH;

};

#endif // __GUIMANAGER_H__
