#include "GuiCheckBox.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "GuiManager.h"

GuiCheckBox::GuiCheckBox(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::CHECKBOX, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;

	check = app->tex->Load("Assets/Textures/check.png");
	box = app->tex->Load("Assets/Textures/box.png");
	lbox.x = bounds.x + 5;
	lbox.y = bounds.y + 5;
	lbox.w = bounds.w - 10;
	lbox.h = bounds.h - 10;

	checked = app->win->fullscreen;
}

GuiCheckBox::~GuiCheckBox()
{

}

bool GuiCheckBox::Update(float dt)
{
	debug = app->guiManager->debug;

	if (checked) {
		app->render->DrawTexture(check, bounds.x + 4 + app->render->camera.x*-1, bounds.y + 4 + app->render->camera.y*-1, NULL,1);
	}

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
				if (checked) {
					checked = false;
				}
				else {
					checked = true;
				}
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
			break;
		case GuiControlState::FOCUSED:
			if (debug)
				app->render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
			SDL_SetRenderDrawColor(app->render->renderer, 255, 255, 255, (Uint8)(125));
			SDL_RenderFillRect(app->render->renderer, &lbox);
			break;
		case GuiControlState::PRESSED:
			if (debug)
				app->render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
			break;
		}
		app->render->DrawTexture(box, bounds.x + app->render->camera.x * -1, bounds.y + app->render->camera.y * -1, NULL);
		app->render->DrawText(text.GetString(), bounds.x - bounds.w / 2 - text.Length() * 25, bounds.y, text.Length() * 25, 50, 255, 255, 255);
	}

	return false;
}

void GuiCheckBox::CleanUp()
{
	app->tex->UnLoad(check);
	app->tex->UnLoad(box);
}
