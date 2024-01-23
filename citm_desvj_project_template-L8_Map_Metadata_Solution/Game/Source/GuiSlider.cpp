#include "GuiSlider.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"

GuiSlider::GuiSlider(uint32 id, SDL_Rect bounds, const char* text, SDL_Rect sliderbounds) : GuiControl(GuiControlType::SLIDER, id)
{
	this->bounds = bounds;
	this->text = text;
	sliderBounds = sliderbounds;

	canClick = true;
	drawBasic = false;

	textW = 100;
	textH = 30;

	percent = 1.0f;
}

GuiSlider::~GuiSlider()
{
}

bool GuiSlider::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		// L15: DONE 3: Update the state of the GUiSlider according to the mouse position
		app->input->GetMousePosition(mouseX, mouseY);

		//If the position of the mouse if inside the bounds of the button 
		if ((mouseX > bounds.x && mouseX < bounds.x + bounds.w && mouseY > bounds.y && mouseY < bounds.y + bounds.h) ||
			(mouseX > sliderBounds.x && mouseX < sliderBounds.x + sliderBounds.w && mouseY > sliderBounds.y && mouseY < sliderBounds.y + sliderBounds.h)) {

			state = GuiControlState::FOCUSED;

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = GuiControlState::PRESSED;
				NotifyObserver();
			}
		}
		else {
			state = GuiControlState::NORMAL;
		}

		app->render->DrawRectangle(bounds, 255, 255, 255, 255, true, false);
		//L15: DONE 4: Draw the button according the GuiControl State
		switch (state)
		{
		case GuiControlState::DISABLED:
			app->render->DrawRectangle(sliderBounds, 200, 200, 200, 255, true, false);
			break;
		case GuiControlState::NORMAL:
			app->render->DrawRectangle(sliderBounds, 0, 0, 255, 255, true, false);
			break;
		case GuiControlState::FOCUSED:
			app->render->DrawRectangle(sliderBounds, 0, 0, 20, 255, true, false);
			break;
		case GuiControlState::PRESSED:
			if (sliderBounds.x + (sliderBounds.w / 2) >= bounds.x &&
				sliderBounds.x + (sliderBounds.w / 2) <= bounds.x + bounds.w) {
				sliderBounds.x = mouseX - sliderBounds.w / 2;
			}
			if ((sliderBounds.x + (sliderBounds.w / 2)) < bounds.x) {
				sliderBounds.x = bounds.x - (sliderBounds.w / 2);
			}
			if ((sliderBounds.x + (sliderBounds.w / 2)) > bounds.x + bounds.w) {
				sliderBounds.x = bounds.x + bounds.w - (sliderBounds.w / 2);
			}
			app->render->DrawRectangle(sliderBounds, 0, 255, 0, 255, true, false);
			float fx = abs((float)(bounds.x - (sliderBounds.x + sliderBounds.w/2)));
			percent = (fx / ((float)bounds.w));
			break;
		}
		SDL_Rect title = { bounds.x + bounds.w / 2 - textW / 2, bounds.y - textH, textW, textH };
		app->render->DrawText(text.GetString(), bounds.x + bounds.w/2 - (text.Length() * 25)/2, bounds.y - 50 - 10, text.Length()*25, 50, 255, 255, 255);
	}
	return false;
}
