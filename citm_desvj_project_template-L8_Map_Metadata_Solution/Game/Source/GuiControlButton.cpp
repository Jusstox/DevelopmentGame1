#include "GuiControlButton.h"
#include "Render.h"
#include "App.h"
#include "GuiManager.h"

GuiControlButton::GuiControlButton(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	debug = app->guiManager->debug;

	if (state != GuiControlState::DISABLED)
	{
		// L15: DONE 3: Update the state of the GUiButton according to the mouse position
		app->input->GetMousePosition(mouseX, mouseY);

		//If the position of the mouse if inside the bounds of the button 
		if (mouseX > bounds.x && mouseX < bounds.x + bounds.w && mouseY > bounds.y && mouseY < bounds.y + bounds.h) {
		
			state = GuiControlState::FOCUSED;

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = GuiControlState::PRESSED;
			}
			
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			state = GuiControlState::NORMAL;
		}

		//L15: DONE 4: Draw the button according the GuiControl State
		switch (state)
		{
		case GuiControlState::DISABLED:
			if (debug)
			app->render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
			break;
		case GuiControlState::NORMAL:
			if (debug)
			app->render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
			app->render->DrawText(text.GetString(), bounds.x, bounds.y, bounds.w, bounds.h, 220, 220, 220);
			anim = false;
			animW = 0;
			animH = 0;
			break;
		case GuiControlState::FOCUSED:
			if (debug)
			app->render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
			app->render->DrawText(text.GetString(), bounds.x - (int)animW, bounds.y - (int)animH, bounds.w + (int)animW*2, bounds.h + (int)animH*2, 255,255,255);
			if (!anim) {
				animW += animSpeed * dt;
				animH += animSpeed * dt;
				if (animW >= 10) {
					anim = !anim;
				}
			}
			else {
				animW -= animSpeed * dt;
				animH -= animSpeed * dt;
				if (animW <= 0) {
					anim = !anim;
				}
			}
			break;
		case GuiControlState::PRESSED:
			if (debug)
			app->render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
			app->render->DrawText(text.GetString(), bounds.x + 5, bounds.y + 5, bounds.w - 10, bounds.h - 10, 25, 25, 25);
			break;
		}

	}
	else {
		if (debug)
		app->render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
		app->render->DrawText(text.GetString(), bounds.x, bounds.y, bounds.w, bounds.h, 50, 50, 50);
	}

	return false;
}

