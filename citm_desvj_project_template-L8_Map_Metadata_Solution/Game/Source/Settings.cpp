#include "Settings.h"
#include "Window.h"
#include "Input.h"
#include "SceneManager.h"
#include "GuiManager.h"
#include "Audio.h"

Settings::Settings() : Scene()
{
	name.Create("settings");
}

Settings::~Settings()
{
}

bool Settings::Awake(pugi::xml_node config)
{
	return true;
}

bool Settings::Start()
{
	app->win->GetWindowSize(windowW, windowH);
	if (app->sceneManager->currentScene == (Scene*)app->sceneManager->menu) {
		SDL_Rect sliderMusic = { windowW / 2 - 200, windowH / 2 - 10 - (windowH / 10) * 2, 400,20 };
		SDL_Rect sliderMusicBounds = { sliderMusic.x + sliderMusic.w - 15, sliderMusic.y + (sliderMusic.h / 2) - 15, 30, 30 };
		gcMSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 6, "MUSIC VOLUME", sliderMusic, this, sliderMusicBounds);

		SDL_Rect sliderPos = { windowW / 2 - 200,  windowH / 2 - 10 - (windowH / 20), 400,20 };
		SDL_Rect sliderBoundsPos = { sliderPos.x + sliderPos.w - 15, sliderPos.y + (sliderPos.h / 2) - 15, 30, 30 };
		gcFSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, "FX VOLUME", sliderPos, this, sliderBoundsPos);

		SDL_Rect checkboxFS = { windowW / 2 - 15 + 150, windowH / 2 - 15 + (windowH / 15), 50,50 };
		checkBoxFS = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 8, "FULL SCREEN", checkboxFS, this);

		SDL_Rect checkboxVS = { windowW / 2 - 15 + 85, windowH / 2 - 15 + (windowH / 10) * 2, 50,50 };
		checkBoxVS = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, "V-SYNC", checkboxVS, this);

		fullscreen = checkBoxFS->checked;
	}
	else {
		SDL_Rect resumePos = { windowW/2 - 75, windowH / 2 - 25 - (windowH / 7) * 2, 150,50 };
		resumeButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, "RESUME", resumePos, this);
		SDL_Rect settingsPos = { windowW/2 - 100, windowH / 2 - 25 - (windowH / 7), 200,50 };
		settingsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "SETTINGS", settingsPos, this);
		SDL_Rect menuPos = { windowW/2 - 50, windowH / 2 - 25, 100,50 };
		menuButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, "MENU", menuPos, this);
		SDL_Rect exitPos = { windowW/2 - 50, windowH / 2 - 25 + (windowH / 7), 100,50 };
		exitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 13, "EXIT", exitPos, this);
	}
	return true;
}

bool Settings::PreUpdate()
{
	return true;
}

bool Settings::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && app->sceneManager->currentScene == (Scene*)app->sceneManager->menu) {
		app->sceneManager->CloseSettings();
	}
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && menuSetings) {
		app->guiManager->DeleteGuiControl(gcMSlider);
		app->guiManager->DeleteGuiControl(gcFSlider);
		checkBoxFS->CleanUp();
		app->guiManager->DeleteGuiControl(checkBoxFS);
		checkBoxVS->CleanUp();
		app->guiManager->DeleteGuiControl(checkBoxVS);
		app->guiManager->ActiveAllGui();
		menuSetings = false;
	}
	if (quit) {
		return false;
	}
	return true;
}

bool Settings::PostUpdate()
{
	return true;
}

bool Settings::CleanUp()
{
	if (app->sceneManager->currentScene == (Scene*)app->sceneManager->menu) {
		app->guiManager->DeleteGuiControl(gcMSlider);
		app->guiManager->DeleteGuiControl(gcFSlider);
		checkBoxFS->CleanUp();
		app->guiManager->DeleteGuiControl(checkBoxFS);
		checkBoxVS->CleanUp();
		app->guiManager->DeleteGuiControl(checkBoxVS);
	}
	else {
		app->guiManager->DeleteGuiControl(resumeButton);
		app->guiManager->DeleteGuiControl(settingsButton);
		app->guiManager->DeleteGuiControl(menuButton);
		app->guiManager->DeleteGuiControl(exitButton);
	}
	return true;
}

bool Settings::OnGuiMouseClickEvent(GuiControl* control)
{
	if (control->id == 8) {
		if (!fullscreen) {
			SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
			fullscreen = true;
		}
		else {
			SDL_SetWindowFullscreen(app->win->window, 0);
			SDL_SetWindowSize(app->win->window, windowW, windowH);
			fullscreen = false;
		}
	}
	if (control->id == 6) {
		app->audio->ChangeMusicVolume(gcMSlider->percent);
	}
	if (control->id == 7) {
		app->audio->ChangeFxVolume(gcFSlider->percent);
	}


	if (control->id == 10) {
		app->sceneManager->CloseSettings();
	}
	if (control->id == 11) {
		app->guiManager->DesactvieAllGui();

		menuSetings = true;

		SDL_Rect sliderMusic = { windowW / 2 - 200, windowH / 2 - 10 - (windowH / 10) * 2, 400,20 };
		SDL_Rect sliderMusicBounds = { sliderMusic.x + sliderMusic.w - 15, sliderMusic.y + (sliderMusic.h / 2) - 15, 30, 30 };
		gcMSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 6, "MUSIC VOLUME", sliderMusic, this, sliderMusicBounds);

		SDL_Rect sliderPos = { windowW / 2 - 200,  windowH / 2 - 10 - (windowH / 20), 400,20 };
		SDL_Rect sliderBoundsPos = { sliderPos.x + sliderPos.w - 15, sliderPos.y + (sliderPos.h / 2) - 15, 30, 30 };
		gcFSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, "FX VOLUME", sliderPos, this, sliderBoundsPos);

		SDL_Rect checkboxFS = { windowW / 2 - 15 + 150, windowH / 2 - 15 + (windowH / 15), 50,50 };
		checkBoxFS = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 8, "FULL SCREEN", checkboxFS, this);

		SDL_Rect checkboxVS = { windowW / 2 - 15 + 85, windowH / 2 - 15 + (windowH / 10) * 2, 50,50 };
		checkBoxVS = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, "V-SYNC", checkboxVS, this);
	}
	if (control->id == 12) {
		app->sceneManager->fade = true;
		app->sceneManager->newScene = (Scene*)app->sceneManager->menu;
		app->sceneManager->currentStep = TO_BLACK;
		app->sceneManager->maxFadeFrames = 100;
		app->sceneManager->CloseSettings();
	}
	if (control->id == 13) {
		quit = true;
	}
	return true;
}
