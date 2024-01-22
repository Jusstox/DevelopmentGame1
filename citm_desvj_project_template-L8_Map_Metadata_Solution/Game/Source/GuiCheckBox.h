#ifndef __GUICHECKBOX_H__
#define __GUICHECKBOX_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"
class GuiCheckBox : public GuiControl
{
public:

	GuiCheckBox(uint32 id, SDL_Rect bounds, const char* text);
	virtual ~GuiCheckBox();

	// Called each loop iteration
	bool Update(float dt);

	void CleanUp();

	bool checked;

private:

	int mouseX, mouseY;
	unsigned int click;

	bool canClick = true;
	bool drawBasic = false;

	SDL_Texture* box;
	SDL_Texture* check;
	SDL_Rect lbox;
	
};

#endif //__GUICHECKBOX_H__