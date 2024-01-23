#include "GuiManager.h"
#include "App.h"
#include "Textures.h"
#include "SceneManager.h"

#include "GuiControlButton.h"
#include "GuiSlider.h"
#include "GuiCheckBox.h"
#include "Audio.h"
#include "Window.h"

GuiManager::GuiManager() :Module()
{
	name.Create("guiManager");
}

GuiManager::~GuiManager() {}

bool GuiManager::Start()
{
	app->win->GetWindowSize(windowW, windowH);
	quat.x = 0;
	quat.y = 0;
	quat.w = windowW;
	quat.h = windowH;
	return true;
}

// L15: DONE1: Implement CreateGuiControl function that instantiates a new GUI control and add it to the list of controls
GuiControl* GuiManager::CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Scene* observer, SDL_Rect sliderBounds)
{
	GuiControl* guiControl = nullptr;

	//Call the constructor according to the GuiControlType
	switch (type)
	{
	case GuiControlType::BUTTON:
		guiControl = new GuiControlButton(id, bounds, text);
		break;
	case GuiControlType::SLIDER:
		guiControl = new GuiSlider(id, bounds, text, sliderBounds);
		break;
	case GuiControlType::CHECKBOX:
		guiControl = new GuiCheckBox(id, bounds, text);
		break;
	}

	//Set the observer
	guiControl->observer = observer;

	// Created GuiControls are add it to the list of controls
	guiControlsList.Add(guiControl);

	return guiControl;
}

bool GuiManager::Update(float dt)
{	
	if (app->sceneManager->currentScene->settings) {
		SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(125));
		SDL_RenderFillRect(app->render->renderer, &quat);
	}
	ListItem<GuiControl*>* control = guiControlsList.start;

	while (control != nullptr)
	{
		if (control->data->active) {
			control->data->Update(dt);
		}
		control = control->next;
	}

	return true;
}

bool GuiManager::CleanUp()
{
	ListItem<GuiControl*>* control = guiControlsList.start;

	while (control != nullptr)
	{
		RELEASE(control);
	}

	return true;

}

bool GuiManager::DeleteGuiControl(GuiControl* guiControl)
{
	ListItem<GuiControl*>* control;
	for (control = guiControlsList.start; control != NULL; control = control->next)
	{
		if (control->data == guiControl) {
			control->data->CleanUp();
			guiControlsList.Del(control);
			break;
		}
	}
	return true;
}

GuiControl* GuiManager::GetFromID(int id)
{
	ListItem<GuiControl*>* control = guiControlsList.start;

	while (control != nullptr)
	{
		if (control->data->id == id)
			return control->data;
		control = control->next;
	}
}

void GuiManager::ActiveAllGui()
{
	ListItem<GuiControl*>* control;
	for (control = guiControlsList.start; control != NULL; control = control->next)
	{
		control->data->active = true;
	}
}

void GuiManager::DesactvieAllGui()
{
	ListItem<GuiControl*>* control;
	for (control = guiControlsList.start; control != NULL; control = control->next)
	{
		control->data->active = false;
	}
}



