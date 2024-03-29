#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "SceneManager.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Window.h"
#include "Map.h"
#include "Level1.h"
#include "Level2.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: DONE 2: Initialize Player parameters
	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
	initPosition = position;

	// get the animations
	// idle animation
	idleAnim.loop = config.child("animations").child("idleanim").attribute("loop").as_bool();
	idleAnim.speed = config.child("animations").child("idleanim").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("idleanim").child("idle"); animNode != NULL; animNode = animNode.next_sibling("idle")) {
		idleAnim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	// walk animation
	walkAnim.loop = config.child("animations").child("walkanim").attribute("loop").as_bool();
	walkAnim.speed = config.child("animations").child("walkanim").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("walkanim").child("walk"); animNode != NULL; animNode = animNode.next_sibling("walk")) {
		walkAnim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}
	
	// die anim
	dieAnim.loop = config.child("animations").child("dieanim").attribute("loop").as_bool();
	dieAnim.speed = config.child("animations").child("dieanim").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("dieanim").child("die"); animNode != NULL; animNode = animNode.next_sibling("die")) {
		dieAnim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	// init jump anim
	jumpAnim1.loop = config.child("animations").child("jumpanim1").attribute("loop").as_bool();

	for (pugi::xml_node animNode = config.child("animations").child("jumpanim1").child("jump"); animNode != NULL; animNode = animNode.next_sibling("jump")) {
		jumpAnim1.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	// final jump anim
	jumpAnim2.loop = config.child("animations").child("jumpanim2").attribute("loop").as_bool();

	for (pugi::xml_node animNode = config.child("animations").child("jumpanim2").child("jump"); animNode != NULL; animNode = animNode.next_sibling("jump")) {
		jumpAnim2.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	blendFadeIN.PushBack({ 0,0,600,600 });
	blendFadeIN.loop = false;
	blendFadeIN.speed = 0.3f;

	blendFadeOut.PushBack({ 0,0,600,600 });
	blendFadeOut.loop = false;
	blendFadeOut.speed = 0.3f;

	shurikenanim.loop = true;
	shurikenanim.PushBack({ 84,0, 15,15 });
	shurikenanim.PushBack({ 102,0,15,15 });
	shurikenanim.speed = 0.1f;

	lvl = 0;

	return true;
}

bool Player::Start() {

	texture = app->tex->Load(config.attribute("texturePath").as_string());
	blendTexture = app->tex->Load(config.attribute("textureblendPath").as_string());
	wonTexture = app->tex->Load(config.attribute("wonpath").as_string());
	loseTexture = app->tex->Load(config.attribute("losepath").as_string());
	app->tex->GetSize(texture, texW, texH);

	currentSAnimation = &shurikenanim;
	currentAnimation = &idleAnim;
	state = IDLE;

	// L07 DONE 5: Add physics to the player - initialize physics body
	if (pbody == NULL) {
		pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 2, bodyType::DYNAMIC);
		// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
		pbody->listener = this;
		// L07 DONE 7: Assign collider type
		pbody->ctype = ColliderType::PLAYER;
	}
	else {
		respawn();
	}
	
	if (pbodyshuriken == NULL) {
		pbodyshuriken = app->physics->CreateCircle(0, 0, 6, bodyType::DYNAMIC);
		pbodyshuriken->body->SetGravityScale(0);
		pbodyshuriken->listener = this;
		pbodyshuriken->ctype = ColliderType::SHURIKEN;
	}
	pbodyshuriken->body->SetActive(true);

	if (pfeet == NULL) {
		pfeet = app->physics->CreateRectangleSensor(position.x + currentAnimation->GetCurrentFrame().w / 2,
			position.y + currentAnimation->GetCurrentFrame().h / 2, 17,
			5, bodyType::DYNAMIC);
		pfeet->listener = this;
	}
	pfeet->body->SetActive(true);
	pfeet->body->SetGravityScale(1);

	//initialize audio effect
	if (pickCoinFxId == -1) {
		pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());
		victory = app->audio->LoadFx(config.attribute("winfxpath").as_string());
		checkfx = app->audio->LoadFx(config.attribute("checkfxpath").as_string());
	}

	speed = config.attribute("speed").as_float();
	death = false;
	flip = false;
	dark = false;
	godmode = false;
	bossing = false;
	fight = false;
	fxbossplayed = false;
	lastcheckpoint = initPosition;
	lastcheckpoint2 = iPoint(32, 1408);
	checkpointposlvl1[0] = iPoint(2544, 740);
	checkpointposlvl1[1] = iPoint(3792, 30);
	checkpointposlvl1[2] = iPoint(6100, 30);
	icheckiterator = 0;
	won = false;
	lose = false;

	return true;
}

bool Player::Update(float dt)
{
	OPTICK_EVENT();
	//dt = dt / 2;
	// L07 DONE 5: Add physics to the player - updated player position using physics
	if (won) {
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			app->sceneManager->fade = true;
			app->sceneManager->newScene = (Scene*)app->sceneManager->menu;
			app->sceneManager->currentStep = TO_BLACK;
			app->sceneManager->maxFadeFrames = 100;
		}
		app->render->DrawTexture(wonTexture, app->render->camera.x * -1, app->render->camera.y * -1, NULL);
		return true;
	}

	if (lose) {
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			app->sceneManager->fade = true;
			app->sceneManager->newScene = (Scene*)app->sceneManager->menu;
			app->sceneManager->currentStep = TO_BLACK;
			app->sceneManager->maxFadeFrames = 100;
		}
		app->render->DrawTexture(loseTexture, app->render->camera.x * -1, app->render->camera.y * -1, NULL);
		return true;
	}

	if (app->sceneManager->currentScene->settings) {
		if (flip) {
			app->render->DrawTexturePR(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
		}
		else {
			app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
		}
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		pbody->body->SetGravityScale(0);
		b2Transform pbodyPos = pbody->body->GetTransform();
		b2Vec2 fpos = b2Vec2(pbodyPos.p.x, pbodyPos.p.y + 0.2);
		pfeet->body->SetTransform(fpos, 0);
		if (dark) {
			app->render->DrawTexturePR(blendTexture, position.x - 300, position.y - 300, &blendFadeIN.GetCurrentFrame());
		}
		pbodyshuriken->body->SetLinearVelocity(b2Vec2(0, 0));
		if (shuriken) {
			b2Transform pbodyPos = pbodyshuriken->body->GetTransform();
			Sposition.x = METERS_TO_PIXELS(pbodyPos.p.x) - 15;
			Sposition.y = METERS_TO_PIXELS(pbodyPos.p.y) - 15;
			app->render->DrawTexture(texture, Sposition.x + currentSAnimation->GetCurrentFrame().w / 2,
				Sposition.y + currentSAnimation->GetCurrentFrame().h / 2, &shurikenanim.GetCurrentFrame());
		}
		return true;
	}
	else {
		if (shuriken) {
			b2Vec2 shirukenvel = b2Vec2(direction.x * 8, direction.y * 8);
			pbodyshuriken->body->SetLinearVelocity(shirukenvel);
		}
		pbody->body->SetGravityScale(1);
	}

	//L03: DONE 4: render the player texture and modify the position of the player using WSAD keys and render the texture
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		if (!godmode) {
			godmode = true;
			if (dark && !bossing)
				dark = false;
			b2Vec2 vel = b2Vec2(0, 0);
			pbody->body->SetLinearVelocity(vel);
			pfeet->body->SetLinearVelocity(vel);
			vel = b2Vec2(-200, -200);
			pbody->body->SetTransform(vel,0);
			pfeet->body->SetTransform(vel, 0);
		}
		else {
			godmode = false;
			position.x += (26 / 2);
			position.y += (40 / 2);
			b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x) , PIXEL_TO_METERS(position.y));
			pbody->body->SetTransform(pPosition, 0);
		}
	}

	std::string s = std::to_string(lives);
	char const* pchar = s.c_str();
	app->render->DrawText("LIVES", 20, 20, 100, 50, 255, 255, 255);
	app->render->DrawText(pchar, 145, 20, 25, 50, 255, 255, 255);

	switch (state)
	{
	case IDLE:
		currentAnimation = &idleAnim;
		break;
	case WALIKING:
		currentAnimation = &walkAnim;
		break;
	case DIE:
		currentAnimation = &dieAnim;
		break;
	case JUMP1:
		currentAnimation = &jumpAnim1;
		break;
	case JUMP2:
		currentAnimation = &jumpAnim2;
		break;
	default:
		break;
	}

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		respawn();
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		if (app->sceneManager->currentScene == app->sceneManager->level1){
			respawn();
		}
		else {
			app->sceneManager->level2->goNextlvl();
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		if (app->sceneManager->currentScene == app->sceneManager->level1) {
			app->sceneManager->level1->goNextlvl();
		}
		else {
			respawn();
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {
		if (lvl == 1) {
			pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(checkpointposlvl1[icheckiterator].x), PIXEL_TO_METERS(checkpointposlvl1[icheckiterator].y)), 0);
			icheckiterator++;
			if (icheckiterator == 3) {
				icheckiterator = 0;
			}
		}
		else {
			if (!bossing) {
				dark = false;
			}
			pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(lastcheckpoint2.x), PIXEL_TO_METERS(lastcheckpoint2.y)), 0);
		}
	}

	if (!shuriken) {
		b2Vec2 diePos = b2Vec2(PIXEL_TO_METERS(-100), PIXEL_TO_METERS(-100));
		pbodyshuriken->body->SetTransform(diePos, 0);
	}
	else {
		b2Transform pbodyPos = pbodyshuriken->body->GetTransform();
		Sposition.x = METERS_TO_PIXELS(pbodyPos.p.x) - 15;
		Sposition.y = METERS_TO_PIXELS(pbodyPos.p.y) - 15;
		app->render->DrawTexture(texture, Sposition.x + currentSAnimation->GetCurrentFrame().w/2,
			Sposition.y + currentSAnimation->GetCurrentFrame().h/2, &shurikenanim.GetCurrentFrame());
		currentSAnimation->Update(dt);
	}

	if (death) {
		state = DIE;
		if (dieAnim.HasFinished()) {
			dieAnim.Reset();
			state = IDLE;
			lives--;
			death = false;
			if (lives <= 0) {
				//game over 
				lose = true;
			}
			else {
				respawnlastcheckpoint();
			}
		}
	}

	if (!death) {

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -speed * dt;
			flip = true;
			if (state == IDLE) {
				state = WALIKING;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = speed * dt;
			flip = false;
			if (state == IDLE) {
				state = WALIKING;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE) {
			if (state == WALIKING) {
				state = IDLE;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !godmode) {
			if (jumps > 0) {
				state = JUMP1;
				jumps--;
				remainingJumpSteps = maxremainingJumpSteps;
				jumpForceReduce = 0;
			}
		}

		if (remainingJumpSteps > 0) {
			force = (pbody->body->GetMass() * 10 / (1 / 30.0));
			force /= 2.7f;
			force -= jumpForceReduce;
			jumpForceReduce = maxremainingJumpSteps - remainingJumpSteps;
			pbody->body->ApplyForceToCenter(b2Vec2(0, -(force * dt)), true);
			remainingJumpSteps--;
			if (jumpForceReduce > force) {
				remainingJumpSteps = 0;
				jumpForceReduce = 0;
			}
		}

		if (remainingJumpSteps <= 0) {
			if (state == JUMP1) {
				jumpAnim1.Reset();
				state = JUMP2;
			}
		}

		//shuriken
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !shuriken) {
			shuriken = true;
			iPoint mousePos;
			app->input->GetMousePosition(mousePos.x, mousePos.y);
			mousePos.x -= app->render->camera.x;
			mousePos.y -= app->render->camera.y;

			iPoint spawnPos;
			spawnPos.x = position.x + currentAnimation->GetCurrentFrame().w / 2;
			spawnPos.y = position.y + currentAnimation->GetCurrentFrame().h / 2;
			if (app->physics->debug) {
				app->render->DrawLine(spawnPos.x,
					spawnPos.y, mousePos.x, mousePos.y, 255, 0, 0, 255, true);
			}

			direction.x = mousePos.x - spawnPos.x;
			direction.y = mousePos.y - spawnPos.y;
			float Module = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
			direction.x /= Module;
			direction.y /= Module;
			spawnPos.x += direction.x * 12;
			spawnPos.y += direction.y * 12;

			b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(spawnPos.x), PIXEL_TO_METERS(spawnPos.y));
			b2Vec2 vec = b2Vec2(position.x + currentAnimation->GetCurrentFrame().w / 2, 10);
			pbodyshuriken->body->SetTransform(pPosition, 0);
		}
	}

	// poner sizes de textura no hardcoded
	if (dark) {
		app->render->DrawTexturePR(blendTexture, position.x - 300, position.y - 300, &blendFadeIN.GetCurrentFrame());
		if (blendFadeIN.HasFinished()) {
			blend = true;
		}
		blendFadeIN.Update(dt);
		blendFadeOut.Reset();
	}
	else if(blend){
		app->render->DrawTexturePR(blendTexture, position.x - 300, position.y - 300, &blendFadeOut.GetCurrentFrame());
		blendFadeOut.Update(dt);
		blendFadeIN.Reset();
		if (blendFadeOut.HasFinished()) {
			blend = false;
		}
	}

	if (!godmode) {
		pbody->body->SetLinearVelocity(velocity);
		b2Transform pbodyPos = pbody->body->GetTransform();

		//preguntar al profe tema camera (el problema es en y) estaba  &blendFadeOut.GetCurrentFrame().h
		position.x = METERS_TO_PIXELS(pbodyPos.p.x) - (26 / 2);
		position.y = METERS_TO_PIXELS(pbodyPos.p.y) - (40 / 2);

		//jump sensor
		b2Vec2 fpos = b2Vec2(pbodyPos.p.x, pbodyPos.p.y + 0.2);
		pfeet->body->SetTransform(fpos,0);
	}
	else {
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y += -speed * 1.5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += speed * 1.5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			position.x += -speed * 1.5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += speed * 1.5 * dt;
		}
	}

	if (flip) {
		app->render->DrawTexturePR(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
	}else{
		app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
	}

	currentAnimation->Update(dt);

	return true;
}

bool Player::CleanUp()
{
	app->tex->UnLoad(texture);
	app->tex->UnLoad(blendTexture);
	app->tex->UnLoad(wonTexture);
	app->tex->UnLoad(loseTexture);
	if (pbody != nullptr) {
		pbody->body->SetActive(false);
		pfeet->body->SetGravityScale(0);
		pbodyshuriken->body->SetActive(false);
	}
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	b2ContactEdge* contact = pbody->body->GetContactList();
	b2Vec2 contactPonts;
	if (contact != nullptr) {
		contactPonts = contact->contact->GetManifold()->localNormal;
	}
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		if (physA == pbody) {
			if (contactPonts.y == 1) {
				remainingJumpSteps = 0;
			}
		}
		if (physA == pfeet) {
			if (state == JUMP2) {
				jumpAnim2.Reset();
				state = IDLE;
			}
			jumps = 2;
		}
		if (physA == pbodyshuriken) {
			shuriken = false;
		}
		break;
	case ColliderType::ITEM:
		app->audio->PlayFx(pickCoinFxId);
		conins++;
		break;
	case ColliderType::VICTORY:
		if (physA == pbody) {
			app->audio->PlayFx(victory);
		}
		app->sceneManager->level1->goNextlvl();
		break;
	case ColliderType::DEATH:
		if (physA == pbody) {
			if (!godmode) {
				death = true;
			}
		}
		if (physA == pbodyshuriken) {
			shuriken = false;
		}
		break;
	case ColliderType::DARK:
		if (physA == pbody) {
			dark = true;
		}
		break;
	case ColliderType::OUTSIDE:
		if (physA == pbody) {
			dark = false;
		}
		break;
	case ColliderType::ENEMY:
		if (physA == pbody) {
			if (!godmode) {
				death = true;
			}
		}
		if (physA == pbodyshuriken) {
			shuriken = false;
		}
		break;
	case ColliderType::ARENA:
		if (physA == pbody) {
			fight = true;
			if (fxbossplayed == false) {
				app->audio->PlayMusic("Assets/Audio/Music/Boss-music.ogg", 0);
				fxbossplayed = true;
			}
			if (lastcheckpoint2 == iPoint(32, 1408)) {
				lastcheckpoint2 = iPoint(310, 91);
			}
		}
		break;
	case ColliderType::CHECKPOINT:
		if (physA == pbody) {
			if (lvl == 1) {
				app->audio->PlayFx(checkfx);
				lastcheckpoint.x = METERS_TO_PIXELS(physB->body->GetTransform().p.x);
				lastcheckpoint.y = METERS_TO_PIXELS(physB->body->GetTransform().p.y);
			}
			else {
				lastcheckpoint2.x = METERS_TO_PIXELS(physB->body->GetTransform().p.x);
				lastcheckpoint2.y = METERS_TO_PIXELS(physB->body->GetTransform().p.y);
			}
		}
		break;
	default:
		break;
	}
}

void Player::respawn()
{
	if (lvl == 1) {
		b2Vec2 initPos = b2Vec2(PIXEL_TO_METERS(initPosition.x), PIXEL_TO_METERS(initPosition.y));
		pbody->body->SetTransform(initPos, 0);
		position = initPosition;

		dark = false;
		pbody->body->SetActive(true);
	}
	else {

		pbody->body->SetActive(true);
		dark = false;
		position.x = 32;
		position.y = 44 * 32;
		position.x += (26 / 2);
		position.y += (40 / 2);
		b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
		pbody->body->SetTransform(pPosition, 0);
	}
	if (fxbossplayed) {
		app->audio->PlayMusic("Assets/Audio/Music/music.ogg", 0);
	}
	if (lose) {
		conins = 0;
		lives = 3;
	}
	fxbossplayed = false;
	won = false;
	lose = false;
}

int Player::getPlayerTileX()
{
	return (position.x + (currentAnimation->GetCurrentFrame().w / 2)) /app->map->getTileWidth();
}

int Player::getPlayerTileY()
{
	return (position.y + (currentAnimation->GetCurrentFrame().h / 2)) / app->map->getTileHieght();
}

bool Player::LoadState(pugi::xml_node& node)
{
	pugi::xml_node PlayerNode = node.child(name.GetString());
	if (lvl != PlayerNode.attribute("lvl").as_int())
	{
		if (lvl == 2) {
			app->sceneManager->level1->hasToReload = true;
			app->sceneManager->fade = true;
			app->sceneManager->newScene = (Scene*)app->sceneManager->level1;
			app->sceneManager->currentStep = TO_BLACK;
			app->sceneManager->maxFadeFrames = 100;
			return true;
		}
		else {
			app->sceneManager->level2->hasToReload = true;
			app->sceneManager->fade = true;
			app->sceneManager->newScene = (Scene*)app->sceneManager->level2;
			app->sceneManager->currentStep = TO_BLACK;
			app->sceneManager->maxFadeFrames = 100;
			return true;
		}
	}
	position.x = PlayerNode.attribute("x").as_int();
	position.y = PlayerNode.attribute("y").as_int();
	dark = PlayerNode.attribute("dark").as_bool();
	position.x += (26 / 2);
	position.y += (40 / 2);
	b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
	pbody->body->SetTransform(pPosition, 0);

	return true;
}

bool Player::SaveState(pugi::xml_node& node)
{
	pugi::xml_node PlayerNode = node.append_child(name.GetString());
	PlayerNode.append_attribute("x").set_value(position.x);
	PlayerNode.append_attribute("y").set_value(position.y);
	PlayerNode.append_attribute("dark").set_value(dark);
	PlayerNode.append_attribute("lvl").set_value(lvl);
	return true;
}

void Player::SetPlayerPos(int x, int y)
{
	pbody->body->SetActive(true);
	pbodyshuriken->body->SetActive(true);
	pfeet->body->SetActive(true);
	dark = false;
	position.x = x;
	position.y = y;
	position.x += (26 / 2);
	position.y += (40 / 2);
	b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
	pbody->body->SetTransform(pPosition, 0);
}

void Player::respawnlastcheckpoint()
{
	if (lvl == 1) {
		b2Vec2 initPos = b2Vec2(PIXEL_TO_METERS(lastcheckpoint.x), PIXEL_TO_METERS(lastcheckpoint.y));
		pbody->body->SetTransform(initPos, 0);
		position = initPosition;
		pbody->body->SetActive(true);
		if (lastcheckpoint == initPosition) {
			dark = false;
		}
	}
	else {
		pbody->body->SetActive(true);
		b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(lastcheckpoint2.x), PIXEL_TO_METERS(lastcheckpoint2.y));
		pbody->body->SetTransform(pPosition, 0);
		if (lastcheckpoint2 == iPoint(32, 1408)) {
			dark = false;
		}
	}
}
