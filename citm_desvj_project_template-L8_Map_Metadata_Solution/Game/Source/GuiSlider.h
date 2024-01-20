#ifndef __GUISLIDER_H__
#define __GUISLIDER_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"
class GuiSlider :  public GuiControl
{
public:

	GuiSlider(uint32 id, SDL_Rect bounds, const char* text, SDL_Rect sliderbounds);
	virtual ~GuiSlider();

	// Called each loop iteration
	bool Update(float dt);

private:

	int mouseX, mouseY;
	unsigned int click;
	SDL_Rect sliderBounds;

	bool canClick = true;
	bool drawBasic = false;
};

#endif  //__GUISLIDER_H__