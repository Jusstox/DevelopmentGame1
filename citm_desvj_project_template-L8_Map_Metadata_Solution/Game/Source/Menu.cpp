#include "Menu.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "SceneManager.h"
#include "Window.h"
#include "Audio.h"
#include "GuiManager.h"
#include "Settings.h"
#include "Level1.h"
#include "Level2.h"

Menu::Menu() : Scene()
{
	name.Create("menu");
}

Menu::~Menu()
{
}

bool Menu::Awake(pugi::xml_node config)
{
	this->config = config;
	return true;
}

bool Menu::Start()
{
	app->win->GetWindowSize(windowW, windowH);
	quat.x = 0;
	quat.y = 0;
	quat.w = windowW;
	quat.h = windowH;
	settings = false;
	img = app->tex->Load(config.attribute("texturePath").as_string());
	app->audio->PlayMusic(config.attribute("musicpath").as_string(),0);
	app->tex->GetSize(img, texW, texH);
	app->render->camera.x = 0;
	app->render->camera.y = 0;
	app->win->GetWindowSize(windowW, windowH);
	// 25px per letter
	SDL_Rect playPos = { windowW / 20, windowH / 2 - 25 - (windowH/7)*2, 200,50 };
	startButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "NEW GAME", playPos, this);
	SDL_Rect continuePos = { windowW / 20, windowH / 2 - 25 - (windowH / 7), 200,50 };
	continueButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "CONTINUE", continuePos, this);
	SDL_Rect settingsPos = { windowW / 20, windowH / 2 - 25, 200,50 };
	settingsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "SETTINGS", settingsPos, this);
	SDL_Rect creditsPos = { windowW / 20, windowH / 2 - 25 + (windowH / 7), 175,50 };
	creditsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "CREDITS", creditsPos, this);
	SDL_Rect exitPos = { windowW / 20, windowH / 2 - 25 + (windowH / 7) * 2, 100,50 };
	exitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "EXIT", exitPos, this);
	pugi::xml_document saveFile;
	pugi::xml_parse_result result = saveFile.load_file("save_game.xml");
	if (!result && saveFile.child("game_state").child("entitymanager").child("Player").attribute("lvl").as_int() == 1) {
		app->guiManager->GetFromID(2)->state = GuiControlState::DISABLED;
	}

	return true;
}

bool Menu::PreUpdate()
{
	return true;
}

bool Menu::Update(float dt)
{
	OPTICK_EVENT();

	app->render->DrawTexture(img, windowW / 2 - texW / 2, windowH / 2 - texH / 2, NULL);
	if (settings) {
		quat.x = app->render->camera.x;
		quat.y = app->render->camera.y;
		SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(125));
		SDL_RenderFillRect(app->render->renderer, &quat);
	}
	return true;
}

bool Menu::PostUpdate()
{
	if (quit)
		return false;
	return true;
}

bool Menu::CleanUp()
{
	app->tex->UnLoad(img);
	app->guiManager->DeleteGuiControl(startButton);
	app->guiManager->DeleteGuiControl(continueButton);
	app->guiManager->DeleteGuiControl(settingsButton);
	app->guiManager->DeleteGuiControl(creditsButton);
	app->guiManager->DeleteGuiControl(exitButton);
	app->guiManager->CleanUp();
	return true;
}

bool Menu::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	if (control->id == 1) {
		app->sceneManager->fade = true;
		app->sceneManager->newScene = (Scene*)app->sceneManager->level1;
		app->sceneManager->currentStep = TO_BLACK;
		app->sceneManager->maxFadeFrames = 100;
		app->sceneManager->level1->reset = true;
	}
	if (control->id == 2) {
		pugi::xml_document saveFile;
		pugi::xml_parse_result result = saveFile.load_file("save_game.xml");
		if (saveFile.child("game_state").child("entitymanager").child("Player").attribute("lvl").as_int() == 1) {
			app->sceneManager->level1->hasToReload = true;
			app->sceneManager->fade = true;
			app->sceneManager->newScene = (Scene*)app->sceneManager->level1;
			app->sceneManager->currentStep = TO_BLACK;
			app->sceneManager->maxFadeFrames = 100;
			app->sceneManager->level1->reset = true;
		}
	}
	if (control->id == 3) {
		app->guiManager->DesactvieAllGui();
		app->sceneManager->OpenSettings();
		settings = true;
	}
	if (control->id == 4) {
		app->sceneManager->level1->reset = false;
		settings = true;
	}
	if (control->id == 5) {
		quit = true;
	}

	return true;
}
